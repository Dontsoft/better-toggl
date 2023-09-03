#ifndef SUMMARIZEDTIMEENTRYMODEL_HPP
#define SUMMARIZEDTIMEENTRYMODEL_HPP

#include <QAbstractItemModel>
#include <QSharedPointer>
#include "billingsummarizer.hpp"
#include "toggl/apitypes.hpp"
#include <optional>

class SimpleConfig;
class GlobalStorage;

class SummarizedTimeEntryModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum UserRoles {
        IsRootItem = Qt::UserRole,
        IsProjectItem = Qt::UserRole + 1,
        IsGroupItem = Qt::UserRole + 2,
        IsEntryItem = Qt::UserRole + 3,
        EntryItem = Qt::UserRole + 10
    };

    explicit SummarizedTimeEntryModel(QSharedPointer<GlobalStorage> adminGlobalStorage,
                                      QSharedPointer<GlobalStorage> globalStorage,
                                      QSharedPointer<SimpleConfig> simpleConfig,
                                      QObject *parent = nullptr);
    ~SummarizedTimeEntryModel() { delete _rootItem; }

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QMap<QPair<QString, QString>, QList<BillingSummarizer::SummarizedTimeEntry>> getExportData()
        const;

public slots:
    void simpleConfigChanged(const QString &key);
    void globalStorageUpdated(const QString &key);
    void refreshData();

private:
    QSharedPointer<GlobalStorage> _adminGlobalStorage;
    QSharedPointer<GlobalStorage> _globalStorage;
    QSharedPointer<SimpleConfig> _simpleConfig;

    struct TreeItem
    {
        TreeItem() {}
        TreeItem(const toggl::Project &_project, TreeItem *_parent)
            : project(_project)
            , parent(_parent)
        {
            parent->children.append(this);
        }

        TreeItem(const QString &_group, TreeItem *_parent)
            : group(_group)
            , parent(_parent)
        {
            parent->children.append(this);
        }

        TreeItem(const BillingSummarizer::SummarizedTimeEntry &_summarizedEntry, TreeItem *_parent)
            : summarizedEntry(_summarizedEntry)
            , parent(_parent)
        {
            parent->children.append(this);
        }

        ~TreeItem() { qDeleteAll(children); }

        TreeItem *parent = nullptr;
        QList<TreeItem *> children;
        std::optional<toggl::Project> project;
        std::optional<QString> group;
        std::optional<BillingSummarizer::SummarizedTimeEntry> summarizedEntry;

        TreeItem *child(int row) const
        {
            if (row < 0 || row >= children.count()) {
                return nullptr;
            }
            return children.at(row);
        }

        int row() const
        {
            if (parent) {
                return parent->children.indexOf(this);
            }
            return 0;
        }

        bool isProjectItem() const { return project.has_value(); }
        bool isGroupItem() const { return group.has_value(); }
        bool isSummarizedEntry() const { return summarizedEntry.has_value(); }
        bool isRootItem() const
        {
            return !isProjectItem() && !isGroupItem() && !isSummarizedEntry();
        }
        int columns() const
        {
            if (isProjectItem() || isGroupItem()) {
                return 3;
            } else if (isSummarizedEntry()) {
                return 3;
            }
            return 1;
        }

        QVariant data(int column)
        {
            if (column >= columns() || column < 0 || isRootItem()) {
                return QVariant();
            }
            if (isProjectItem()) {
                switch (column) {
                case 0:
                    return tr("%1 (%2 Groups)")
                        .arg(project.value().get<QString>(toggl::Project::NAME).value())
                        .arg(children.count());
                case 1: {
                    QDateTime minDateTime, maxDateTime;
                    QString minDateDisplay, maxDateDisplay;
                    for (const auto &group : children) {
                        for (const auto &child : group->children) {
                            if (minDateTime.isNull()
                                || minDateTime >= child->summarizedEntry->timeFrameKey) {
                                minDateTime = child->summarizedEntry->timeFrameKey;
                                minDateDisplay = child->summarizedEntry->timeFrameKeyFormatted();
                            }
                            if (maxDateTime.isNull()
                                || maxDateTime <= child->summarizedEntry->timeFrameKey) {
                                maxDateTime = child->summarizedEntry->timeFrameKey;
                                maxDateDisplay = child->summarizedEntry->timeFrameKeyFormatted();
                            }
                        }
                    }

                    return QString("%1 - %2").arg(minDateDisplay, maxDateDisplay);
                }
                case 2: {
                    BillingSummarizer::SummarizedTimeEntry projectSumEntry{
                        QDateTime(),
                        BillingSummarizer::SummarizedTimeEntry::TimeFrameKeyDisplay::DayOnly,
                        QString(),
                        0};
                    for (const auto &group : children) {
                        for (const auto &child : group->children) {
                            projectSumEntry.seconds += child->summarizedEntry->seconds;
                        }
                    }
                    if (projectSumEntry.seconds == 0) {
                        return QVariant();
                    }
                    return QString("%1 / %2").arg(projectSumEntry.format(),
                                                  projectSumEntry.formatDecimal());
                }
                }
            } else if (isGroupItem()) {
                switch (column) {
                case 0:
                    return tr("%1 (%2 Entries)").arg(group.value()).arg(children.count());
                case 1: {
                    QDateTime minDateTime, maxDateTime;
                    QString minDateDisplay, maxDateDisplay;
                    for (const auto &child : children) {
                        if (minDateTime.isNull()
                            || minDateTime >= child->summarizedEntry->timeFrameKey) {
                            minDateTime = child->summarizedEntry->timeFrameKey;
                            minDateDisplay = child->summarizedEntry->timeFrameKeyFormatted();
                        }
                        if (maxDateTime.isNull()
                            || maxDateTime <= child->summarizedEntry->timeFrameKey) {
                            maxDateTime = child->summarizedEntry->timeFrameKey;
                            maxDateDisplay = child->summarizedEntry->timeFrameKeyFormatted();
                        }
                    }
                    return QString("%1 - %2").arg(minDateDisplay, maxDateDisplay);
                }
                case 2: {
                    BillingSummarizer::SummarizedTimeEntry groupSumEntry{
                        QDateTime(),
                        BillingSummarizer::SummarizedTimeEntry::TimeFrameKeyDisplay::DayOnly,
                        QString(),
                        0};
                    for (const auto &child : children) {
                        groupSumEntry.seconds += child->summarizedEntry->seconds;
                    }
                    if (groupSumEntry.seconds == 0) {
                        return QVariant();
                    }
                    return QString("%1 / %2").arg(groupSumEntry.format(),
                                                  groupSumEntry.formatDecimal());
                }
                }
            } else if (isSummarizedEntry()) {
                switch (column) {
                case 0:
                    return summarizedEntry.value().timeFrameKeyFormatted();
                case 1:
                    return summarizedEntry.value().groupingKey;
                case 2:
                    return QString("%1 / %2").arg(summarizedEntry.value().format(),
                                                  summarizedEntry.value().formatDecimal());
                }
            }
            return QVariant();
        }
    };
    TreeItem *_rootItem;
};

#endif // SUMMARIZEDTIMEENTRYMODEL_HPP
