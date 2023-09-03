#include "billingsummarizerwidget.hpp"
#include "ui_billingsummarizerwidget.h"

#include <QHBoxLayout>
#include <QLayoutItem>
#include <QPushButton>

#include "sortablelistlayout.hpp"
#include "splitbyrulewidget.hpp"

BillingSummarizerWidget::BillingSummarizerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BillingSummarizerWidget)
{
    ui->setupUi(this);
    ui->customRuleWidget->setLayout(new SortableListLayout());
    connect(ui->groupByComboBox,
            &QComboBox::currentIndexChanged,
            this,
            &BillingSummarizerWidget::groupByComboBoxChanged);
    connect(ui->groupByTimespanComboBox,
            &QComboBox::currentIndexChanged,
            this,
            &BillingSummarizerWidget::groupByTimespanComboBoxChanged);
    connect(ui->splitByComboBox,
            &QComboBox::currentIndexChanged,
            this,
            &BillingSummarizerWidget::splitByComboBoxChanged);
    connect(ui->addSplitByRuleButton,
            &QPushButton::clicked,
            this,
            &BillingSummarizerWidget::addSplitByRule);
    connect(ui->useWorkspaceRoundingCheckBox,
            &QCheckBox::stateChanged,
            this,
            &BillingSummarizerWidget::useWorkspaceRoundingChanged);
}

BillingSummarizerWidget::~BillingSummarizerWidget()
{
    delete ui;
}

BillingSummarizer BillingSummarizerWidget::getSummarizer() const
{
    BillingSummarizer summarizer;
    summarizer.setGroupingType(
        static_cast<BillingSummarizer::GroupingType>(ui->groupByComboBox->currentIndex()));
    summarizer.setGroupingTimeFrame(static_cast<BillingSummarizer::GroupingTimeFrame>(
        ui->groupByTimespanComboBox->currentIndex()));
    summarizer.setSplitBy(
        static_cast<BillingSummarizer::SplitBy>(ui->splitByComboBox->currentIndex()));
    summarizer.setUseWorkspaceRounding(ui->useWorkspaceRoundingCheckBox->isChecked());
    summarizer.setRounding(static_cast<toggl::Rounding>(ui->roundingComboBox->currentIndex()));
    summarizer.setRoundingMinute(
        static_cast<toggl::RoundingMinute>(ui->roundingMinutesComboBox->currentIndex()));
    if (summarizer.splitBy() == BillingSummarizer::SplitBy::SPLIT_BY_RULE) {
        QList<BillingSummarizer::SplitByRule> rules;
        for (int i = 0; i < ui->customRuleWidget->layout()->count(); ++i) {
            auto item = ui->customRuleWidget->layout()->itemAt(i);
            auto widget = dynamic_cast<SplitByRuleWidget *>(item->widget());
            if (widget) {
                rules.append(widget->getRule());
            }
        }
        summarizer.setCustomSplitByRules(rules);
    }
    auto groupNaming = _groupNamingCache;
    groupNaming.insert(static_cast<BillingSummarizer::SplitBy>(ui->splitByComboBox->currentIndex()),
                       getCurrentMapping());
    summarizer.setGroupNaming(groupNaming);
    return summarizer;
}

void BillingSummarizerWidget::setSummarizer(const BillingSummarizer &summarizer)
{
    ui->groupByComboBox->setCurrentIndex(static_cast<int>(summarizer.groupingType()));
    ui->groupByTimespanComboBox->setCurrentIndex(static_cast<int>(summarizer.groupingTimeFrame()));
    ui->useWorkspaceRoundingCheckBox->setChecked(summarizer.useWorkspaceRounding());
    ui->roundingComboBox->setCurrentIndex(static_cast<int>(summarizer.rounding()));
    ui->roundingMinutesComboBox->setCurrentIndex(static_cast<int>(summarizer.roundingMinute()));
    ui->splitByComboBox->setCurrentIndex(static_cast<int>(summarizer.splitBy()));
    _previousSplit = summarizer.splitBy();
    for (int i = 0; i < ui->customRuleWidget->layout()->count(); ++i) {
        auto item = ui->customRuleWidget->layout()->takeAt(i);
        item->widget()->deleteLater();
        delete item;
    }
    for (const auto &splitByRule : summarizer.customSplitByRules()) {
        const auto layout = dynamic_cast<SortableListLayout *>(ui->customRuleWidget->layout());
        if (layout) {
            auto widget = new SplitByRuleWidget();
            widget->setRule(splitByRule);
            connect(layout,
                    &SortableListLayout::orderChanged,
                    widget,
                    &SplitByRuleWidget::layoutOrderUpdated);
            connect(widget, &SplitByRuleWidget::moveUp, this, &BillingSummarizerWidget::moveUp);
            connect(widget, &SplitByRuleWidget::moveDown, this, &BillingSummarizerWidget::moveDown);
            connect(widget, &SplitByRuleWidget::remove, this, &BillingSummarizerWidget::remove);
            layout->addWidget(widget);
        }
    }
    const auto groupNameMapping = summarizer.groupNaming();
    _groupNamingCache = groupNameMapping;
    regenerateNamingForm();
}

void BillingSummarizerWidget::groupByComboBoxChanged(int index) {}

void BillingSummarizerWidget::groupByTimespanComboBoxChanged(int index) {}

void BillingSummarizerWidget::splitByComboBoxChanged(int index)
{
    if (static_cast<BillingSummarizer::SplitBy>(index)
        == BillingSummarizer::SplitBy::SPLIT_BY_RULE) {
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        ui->stackedWidget->setCurrentIndex(0);
    }
    auto currentMapping = getCurrentMapping();
    _groupNamingCache.insert(_previousSplit, currentMapping);
    regenerateNamingForm();
    _previousSplit = static_cast<BillingSummarizer::SplitBy>(index);
}

void BillingSummarizerWidget::useWorkspaceRoundingChanged(int state)
{
    ui->roundingConfig->setHidden(ui->useWorkspaceRoundingCheckBox->isChecked());
}

void BillingSummarizerWidget::addSplitByRule()
{
    const auto layout = dynamic_cast<SortableListLayout *>(ui->customRuleWidget->layout());
    if (layout) {
        auto widget = new SplitByRuleWidget();
        connect(layout,
                &SortableListLayout::orderChanged,
                widget,
                &SplitByRuleWidget::layoutOrderUpdated);
        connect(widget, &SplitByRuleWidget::moveUp, this, &BillingSummarizerWidget::moveUp);
        connect(widget, &SplitByRuleWidget::moveDown, this, &BillingSummarizerWidget::moveDown);
        connect(widget, &SplitByRuleWidget::remove, this, &BillingSummarizerWidget::remove);

        layout->addWidget(widget);
        emit layout->orderChanged();
        regenerateNamingForm();
    }
}

void BillingSummarizerWidget::moveUp()
{
    auto widget = dynamic_cast<SplitByRuleWidget *>(sender());
    const auto layout = dynamic_cast<SortableListLayout *>(ui->customRuleWidget->layout());
    if (!widget || !layout) {
        return;
    }
    auto [formerIndex, index] = layout->moveWidgetUp(widget);
    switchPlacesInNamingForm(formerIndex, index);
}

void BillingSummarizerWidget::moveDown()
{
    auto widget = dynamic_cast<SplitByRuleWidget *>(sender());
    const auto layout = dynamic_cast<SortableListLayout *>(ui->customRuleWidget->layout());
    if (!widget || !layout) {
        return;
    }
    auto [formerIndex, index] = layout->moveWidgetDown(widget);
    switchPlacesInNamingForm(formerIndex, index);
}

void BillingSummarizerWidget::remove()
{
    auto widget = dynamic_cast<SplitByRuleWidget *>(sender());
    const auto layout = dynamic_cast<SortableListLayout *>(ui->customRuleWidget->layout());
    if (!widget || !layout) {
        return;
    }
    layout->removeWidget(widget);
    widget->deleteLater();
    emit layout->orderChanged();
    regenerateNamingForm();
}

void BillingSummarizerWidget::regenerateNamingForm()
{
    auto splitBy = static_cast<BillingSummarizer::SplitBy>(ui->splitByComboBox->currentIndex());

    auto cachedMapping = _groupNamingCache.value(splitBy);

    switch (splitBy) {
    case BillingSummarizer::SplitBy::DO_NOT_SPLIT: {
        auto it = std::find_if(std::begin(cachedMapping),
                               std::end(cachedMapping),
                               [=](const QPair<QString, QString> &singleMapping) {
                                   return singleMapping.first == QObject::tr("Group 1");
                               });
        QPair<QString, QString> initialValue;
        if (it == std::end(cachedMapping)) {
            cachedMapping.clear();
            cachedMapping.append(qMakePair(QObject::tr("Group 1"), QObject::tr("Group 1")));
        }
        break;
    }
    case BillingSummarizer::SplitBy::SPLIT_BY_TASK: {
        for (const auto &task : _tasks) {
            const auto taskName = task.get<QString>(toggl::Task::NAME).value_or("");
            auto it = std::find_if(std::begin(cachedMapping),
                                   std::end(cachedMapping),
                                   [=](const QPair<QString, QString> &singleMapping) {
                                       return singleMapping.first == taskName;
                                   });
            if (it == std::end(cachedMapping)) {
                cachedMapping.append(qMakePair(taskName, taskName));
            }
        }
        cachedMapping.removeIf([=](const auto &singleMapping) {
            auto it = std::find_if(std::begin(_tasks),
                                   std::end(_tasks),
                                   [=](const toggl::Task &task) {
                                       return task.get<QString>(toggl::Task::NAME).value_or("")
                                              == singleMapping.first;
                                   });
            return it == std::end(_tasks);
        });
        break;
    }
    case BillingSummarizer::SplitBy::SPLIT_BY_USER: {
        for (const auto &user : _users) {
            const auto userName = user.get<QString>(toggl::ReportUser::FULLNAME).value_or("");
            auto it = std::find_if(std::begin(cachedMapping),
                                   std::end(cachedMapping),
                                   [=](const QPair<QString, QString> &singleMapping) {
                                       return singleMapping.first == userName;
                                   });
            if (it == std::end(cachedMapping)) {
                cachedMapping.append(qMakePair(userName, userName));
            }
        }
        cachedMapping.removeIf([=](const auto &singleMapping) {
            auto it = std::find_if(std::begin(_users),
                                   std::end(_users),
                                   [=](const toggl::ReportUser &user) {
                                       return user.get<QString>(toggl::ReportUser::FULLNAME)
                                                  .value_or("")
                                              == singleMapping.first;
                                   });
            return it == std::end(_users);
        });
        break;
    }
    case BillingSummarizer::SplitBy::SPLIT_BY_RULE: {
        const int ruleCount = ui->customRuleWidget->layout()->count();
        if (cachedMapping.count() > ruleCount) {
            cachedMapping.erase(std::cbegin(cachedMapping) + ruleCount, std::cend(cachedMapping));
        } else if (cachedMapping.count() < ruleCount) {
            for (int i = cachedMapping.count(); i < ruleCount; ++i) {
                cachedMapping.append(qMakePair(QObject::tr("Group %1").arg(i + 1),
                                               QObject::tr("Group %1").arg(i + 1)));
            }
        }
        for (int i = 0; i < cachedMapping.count(); ++i) {
            cachedMapping[i].first = QObject::tr("Group %1").arg(i + 1);
        }
        break;
    }
    }
    _groupNamingCache.insert(splitBy, cachedMapping);
    setMapping(cachedMapping);
}
void BillingSummarizerWidget::switchPlacesInNamingForm(int formerIndex, int toIndex)
{
    QList<QPair<QString, QString>> mapping = _groupNamingCache.value(
        BillingSummarizer::SplitBy::SPLIT_BY_RULE);
    auto item = mapping.takeAt(formerIndex);
    mapping.insert(toIndex, item);
    _groupNamingCache.insert(BillingSummarizer::SplitBy::SPLIT_BY_RULE, mapping);
    regenerateNamingForm();
}

QList<QPair<QString, QString>> BillingSummarizerWidget::getCurrentMapping() const
{
    QList<QPair<QString, QString>> currentMapping;
    for (int i = 0; i < ui->renamingTableWidget->rowCount(); ++i) {
        QString key, value;
        auto keyItem = ui->renamingTableWidget->item(i, 0);
        auto valueItem = ui->renamingTableWidget->item(i, 1);
        if (keyItem) {
            key = keyItem->text();
        }
        if (valueItem) {
            value = valueItem->text();
        }
        currentMapping.append(qMakePair(key, value));
    }
    return currentMapping;
}

void BillingSummarizerWidget::setMapping(const QList<QPair<QString, QString>> &mapping)
{
    ui->renamingTableWidget->clearContents();
    ui->renamingTableWidget->setRowCount(0);
    for (auto it = mapping.begin(); it != mapping.end(); ++it) {
        ui->renamingTableWidget->insertRow(ui->renamingTableWidget->rowCount());
        auto keyItem = new QTableWidgetItem(it->first);
        keyItem->setFlags(keyItem->flags() & ~Qt::ItemIsEditable);
        ui->renamingTableWidget->setItem(ui->renamingTableWidget->rowCount() - 1, 0, keyItem);

        auto valueItem = new QTableWidgetItem(it->second);
        ui->renamingTableWidget->setItem(ui->renamingTableWidget->rowCount() - 1, 1, valueItem);
    }
}

void BillingSummarizerWidget::setTasks(const QList<toggl::Task> &newTasks)
{
    _tasks = newTasks;
}

void BillingSummarizerWidget::setUsers(const QList<toggl::ReportUser> &newUsers)
{
    _users = newUsers;
}
