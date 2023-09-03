#include "summarizedtimeentrymodel.hpp"

#include "globalstorage.hpp"
#include "simpleconfig.hpp"

SummarizedTimeEntryModel::SummarizedTimeEntryModel(QSharedPointer<GlobalStorage> adminGlobalStorage,
                                                   QSharedPointer<GlobalStorage> globalStorage,
                                                   QSharedPointer<SimpleConfig> simpleConfig,
                                                   QObject *parent)
    : QAbstractItemModel(parent)
    , _adminGlobalStorage(adminGlobalStorage)
    , _globalStorage(globalStorage)
    , _simpleConfig(simpleConfig)
    , _rootItem{new TreeItem()}
{
    connect(_adminGlobalStorage.data(),
            &GlobalStorage::notify,
            this,
            &SummarizedTimeEntryModel::globalStorageUpdated);
    connect(_simpleConfig.data(),
            &SimpleConfig::changed,
            this,
            &SummarizedTimeEntryModel::simpleConfigChanged);
}

QVariant SummarizedTimeEntryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Time Key");
        case 1:
            return tr("Time Entry");
        case 2:
            return tr("Time (h) / Time (decimal)");
        }
    }
    return QVariant();
}

QModelIndex SummarizedTimeEntryModel::index(int row, int column, const QModelIndex &parent) const
{
    TreeItem *parentItem;

    if (!parent.isValid()) {
        parentItem = _rootItem;
    } else {
        parentItem = static_cast<TreeItem *>(parent.internalPointer());
    }

    TreeItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex SummarizedTimeEntryModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    TreeItem *childItem = static_cast<TreeItem *>(index.internalPointer());
    TreeItem *parentItem = childItem->parent;

    if (parentItem == _rootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int SummarizedTimeEntryModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<TreeItem *>(parent.internalPointer());

    return parentItem->children.count();
}

int SummarizedTimeEntryModel::columnCount(const QModelIndex &parent) const
{
    return 3;
    if (parent.isValid())
        return static_cast<TreeItem *>(parent.internalPointer())->columns();
    return _rootItem->columns();
}

QVariant SummarizedTimeEntryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == Qt::DisplayRole) {
        TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
        return item->data(index.column());
    } else if (role == SummarizedTimeEntryModel::IsRootItem) {
        TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
        return item->isRootItem();
    } else if (role == SummarizedTimeEntryModel::IsProjectItem) {
        TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
        return item->isProjectItem();
    } else if (role == SummarizedTimeEntryModel::IsGroupItem) {
        TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
        return item->isGroupItem();
    } else if (role == SummarizedTimeEntryModel::IsEntryItem) {
        TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
        return item->isSummarizedEntry();
    } else if (role == SummarizedTimeEntryModel::EntryItem) {
        TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
        return QVariant::fromValue(item->summarizedEntry.value());
    }
    return QVariant();
}

Qt::ItemFlags SummarizedTimeEntryModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QMap<QPair<QString, QString>, QList<BillingSummarizer::SummarizedTimeEntry>>
SummarizedTimeEntryModel::getExportData() const
{
    QMap<QPair<QString, QString>, QList<BillingSummarizer::SummarizedTimeEntry>> result;

    for (const auto projectItem : _rootItem->children) {
        auto project = projectItem->project.value();
        auto projectName = project.get<QString>(toggl::Project::NAME).value();
        for (const auto groupItem : projectItem->children) {
            auto group = groupItem->group.value();
            QList<BillingSummarizer::SummarizedTimeEntry> entries;
            for (const auto entryItem : groupItem->children) {
                entries.append(entryItem->summarizedEntry.value());
            }
            result.insert(qMakePair(projectName, group), entries);
        }
    }

    return result;
}

void SummarizedTimeEntryModel::refreshData()
{
    beginResetModel();
    delete _rootItem;
    _rootItem = new TreeItem();

    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto hideEmptyProjects = _simpleConfig->get(SimpleConfig::BILLING_HIDE_EMPTY_PROJECTS, true)
                                 .toBool();
    auto workspace = _globalStorage->workspace(workspaceId);
    int roundingInt = static_cast<int>(toggl::Rounding::NO_ROUNDING);
    int roundingMinutesInt = static_cast<int>(toggl::RoundingMinute::MINUTES_0);
    if (workspace.has_value()) {
        roundingInt = workspace.value().get<int>(toggl::Workspace::ROUNDING).value_or(roundingInt);
        roundingMinutesInt = workspace.value()
                                 .get<int>(toggl::Workspace::ROUNDING_MINUTES)
                                 .value_or(roundingMinutesInt);
    }

    auto projects = _adminGlobalStorage->projects();
    std::sort(std::begin(projects),
              std::end(projects),
              [=](const toggl::Project &a, const toggl::Project &b) {
                  return a.get<QString>(toggl::Project::NAME)
                         < b.get<QString>(toggl::Project::NAME);
              });
    auto tasks = _adminGlobalStorage->tasks();
    auto users = _adminGlobalStorage->users();
    auto summarizers = _adminGlobalStorage->billingSummarizer();
    auto timeEntries = qvariant_cast<toggl::TimeEntryList>(
                           _adminGlobalStorage->get(GlobalStorage::DETAILS))
                           .billable();
    for (const auto &project : projects) {
        auto projectId = project.get<int>(toggl::Project::ID).value_or(-1);
        if (summarizers.contains(projectId)) {
            auto timeEntriesForProject = timeEntries.forProject(projectId);
            auto summarized = summarizers[projectId].summarize(timeEntriesForProject,
                                                               projects,
                                                               tasks,
                                                               users,
                                                               static_cast<toggl::Rounding>(
                                                                   roundingInt),
                                                               static_cast<toggl::RoundingMinute>(
                                                                   roundingMinutesInt));
            bool anyEntries = false;
            QStringList skipGroups;
            if (hideEmptyProjects) {
                for (auto it = summarized.begin(); it != summarized.end(); ++it) {
                    if (it.value().empty()) {
                        skipGroups.append(it.key());
                    } else {
                        anyEntries = true;
                    }
                }
            } else {
                anyEntries = true;
            }
            if (anyEntries) {
                auto projectItem = new TreeItem(project, _rootItem);
                for (auto it = summarized.begin(); it != summarized.end(); ++it) {
                    if (skipGroups.contains(it.key())) {
                        continue;
                    }
                    auto groupItem = new TreeItem(it.key(), projectItem);
                    for (const auto &entry : it.value()) {
                        new TreeItem(entry, groupItem);
                    }
                }
            }
        }
    }

    endResetModel();
}

void SummarizedTimeEntryModel::simpleConfigChanged(const QString &key)
{
    if (key == SimpleConfig::BILLING_HIDE_EMPTY_PROJECTS) {
        refreshData();
    }
}

void SummarizedTimeEntryModel::globalStorageUpdated(const QString &key)
{
    if (key == GlobalStorage::DETAILS || key == GlobalStorage::BILLING_SUMMARIZER
        || key == GlobalStorage::PROJECTS) {
        refreshData();
    }
}
