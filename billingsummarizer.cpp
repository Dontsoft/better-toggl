#include "billingsummarizer.hpp"

#include <QJsonArray>

BillingSummarizer::BillingSummarizer() {}

void BillingSummarizer::fromJson(const QJsonObject &object)
{
    _groupingType = static_cast<GroupingType>(object["grouping_type"].toInt());
    _groupingTimeFrame = static_cast<GroupingTimeFrame>(object["grouping_time_frame"].toInt());
    _splitBy = static_cast<SplitBy>(object["split_by"].toInt());
    _useWorkspaceRounding = object["workspace_rounding"].toBool(true);
    _rounding = static_cast<toggl::Rounding>(object["rounding"].toInt());
    _roundingMinute = static_cast<toggl::RoundingMinute>(object["rounding_minute"].toInt());
    _customSplitByRules.clear();
    QJsonArray arr = object["custom_split_by_rules"].toArray();
    for (const auto &value : arr) {
        QJsonObject ruleObject = value.toObject();
        _customSplitByRules.append(
            SplitByRule{.ruleColumn = static_cast<SplitByRule::Column>(ruleObject["column"].toInt()),
                        .ruleOperator = static_cast<SplitByRule::Operator>(
                            ruleObject["operator"].toInt()),
                        .ruleComparisonValue = ruleObject["comparison_value"].toString()});
    }
    _groupNaming.clear();
    for (const auto &splitByValue : {SplitBy::DO_NOT_SPLIT,
                                     SplitBy::SPLIT_BY_RULE,
                                     SplitBy::SPLIT_BY_TASK,
                                     SplitBy::SPLIT_BY_USER}) {
        QJsonArray arr2 = object[QString("group_naming_%1").arg(static_cast<int>(splitByValue))]
                              .toArray();
        QList<QPair<QString, QString>> splitByMappingValues;
        for (const auto &value : arr2) {
            QJsonObject groupObject = value.toObject();
            splitByMappingValues.append(
                qMakePair(groupObject["group"].toString(), groupObject["label"].toString()));
        }
        _groupNaming.insert(splitByValue, splitByMappingValues);
    }
}

QJsonObject BillingSummarizer::toJson() const
{
    QJsonObject obj;
    obj["grouping_type"] = static_cast<int>(_groupingType);
    obj["grouping_time_frame"] = static_cast<int>(_groupingTimeFrame);
    obj["split_by"] = static_cast<int>(_splitBy);
    obj["workspace_rounding"] = _useWorkspaceRounding;
    obj["rounding"] = static_cast<int>(_rounding);
    obj["rounding_minute"] = static_cast<int>(_roundingMinute);
    QJsonArray arr;
    for (const auto &rule : _customSplitByRules) {
        QJsonObject ruleObj;
        ruleObj["column"] = static_cast<int>(rule.ruleColumn);
        ruleObj["operator"] = static_cast<int>(rule.ruleOperator);
        ruleObj["comparison_value"] = rule.ruleComparisonValue;
        arr.append(ruleObj);
    }
    obj["custom_split_by_rules"] = arr;
    for (const auto &splitByValue : {SplitBy::DO_NOT_SPLIT,
                                     SplitBy::SPLIT_BY_RULE,
                                     SplitBy::SPLIT_BY_TASK,
                                     SplitBy::SPLIT_BY_USER}) {
        QJsonArray arr2;
        const auto mappingList = _groupNaming.value(splitByValue);
        for (auto it = mappingList.begin(); it != mappingList.end(); ++it) {
            QJsonObject groupObject;
            groupObject["group"] = it->first;
            groupObject["label"] = it->second;
            arr2.append(groupObject);
        }
        obj[QString("group_naming_%1").arg(static_cast<int>(splitByValue))] = arr2;
    }
    return obj;
}

void BillingSummarizer::setGroupingType(GroupingType newGroupingType)
{
    _groupingType = newGroupingType;
}

BillingSummarizer::GroupingType BillingSummarizer::groupingType() const
{
    return _groupingType;
}

BillingSummarizer::GroupingTimeFrame BillingSummarizer::groupingTimeFrame() const
{
    return _groupingTimeFrame;
}

void BillingSummarizer::setGroupingTimeFrame(GroupingTimeFrame newGroupingTimeFrame)
{
    _groupingTimeFrame = newGroupingTimeFrame;
}

BillingSummarizer::SplitBy BillingSummarizer::splitBy() const
{
    return _splitBy;
}

void BillingSummarizer::setSplitBy(SplitBy newSplitBy)
{
    _splitBy = newSplitBy;
}

QList<BillingSummarizer::SplitByRule> BillingSummarizer::customSplitByRules() const
{
    return _customSplitByRules;
}

void BillingSummarizer::setCustomSplitByRules(const QList<SplitByRule> &newCustomSplitByRules)
{
    _customSplitByRules = newCustomSplitByRules;
}

QMap<BillingSummarizer::SplitBy, QList<QPair<QString, QString>>> BillingSummarizer::groupNaming() const
{
    return _groupNaming;
}

void BillingSummarizer::setGroupNaming(QMap<SplitBy, QList<QPair<QString, QString>>> &newGroupNaming)
{
    _groupNaming = newGroupNaming;
}

BillingSummarizer::Result BillingSummarizer::summarize(
    const toggl::TimeEntryList &timeEntryList,
    const QList<toggl::Project> &projects,
    const QList<toggl::Task> &tasks,
    const QList<toggl::ReportUser> &users,
    toggl::Rounding workspaceRounding,
    toggl::RoundingMinute workspaceRoundingMinute) const
{
    auto roundedEntries = timeEntryList.rounded(_useWorkspaceRounding ? workspaceRounding
                                                                      : _rounding,
                                                _useWorkspaceRounding ? workspaceRoundingMinute
                                                                      : _roundingMinute);
    QMap<QString, toggl::TimeEntryList> splittedEntries;
    switch (_splitBy) {
    case SplitBy::SPLIT_BY_TASK:
        splittedEntries = splitByTask(roundedEntries, tasks);
        break;
    case SplitBy::SPLIT_BY_USER:
        splittedEntries = splitByUser(roundedEntries, users);
        break;
    case SplitBy::SPLIT_BY_RULE:
        splittedEntries = splitByRule(roundedEntries, tasks, users);
        break;
    case SplitBy::DO_NOT_SPLIT:
    default:
        splittedEntries = {{getGroupNaming(QObject::tr("Group 1")), roundedEntries}};
        break;
    }
    Result result;
    for (auto it = splittedEntries.begin(); it != splittedEntries.end(); ++it) {
        switch (_groupingTimeFrame) {
        case GroupingTimeFrame::DAY:
            result.insert(it.key(), summarizeByDay(it.value(), tasks));
            break;
        case GroupingTimeFrame::MONTH:
            result.insert(it.key(), summarizeByMonth(it.value(), tasks));
            break;
        default:
            result.insert(it.key(), summarizeByTyoe(it.value(), tasks));
            break;
        }
    }
    return result;
}

QMap<QString, toggl::TimeEntryList> BillingSummarizer::splitByTask(
    const toggl::TimeEntryList &timeEntryList, const QList<toggl::Task> &tasks) const
{
    QMap<QString, toggl::TimeEntryList> splitted;
    for (const auto &entry : timeEntryList) {
        const auto taskIt = std::find_if(std::begin(tasks),
                                         std::end(tasks),
                                         [=](const toggl::Task &task) {
                                             return task.get<int>(toggl::Task::ID)
                                                    == entry.get<int>(toggl::TimeEntry::TASK_ID);
                                         });
        QString key;
        if (taskIt != std::end(tasks)) {
            key = getGroupNaming(taskIt->get<QString>(toggl::Task::NAME).value_or(""));
        }
        if (!splitted.contains(key)) {
            splitted.insert(key, {});
        }
        splitted[key].append(entry);
    }
    return splitted;
}

QMap<QString, toggl::TimeEntryList> BillingSummarizer::splitByUser(
    const toggl::TimeEntryList &timeEntryList, const QList<toggl::ReportUser> &users) const
{
    QMap<QString, toggl::TimeEntryList> splitted;
    for (const auto &entry : timeEntryList) {
        const auto userIt = std::find_if(std::begin(users),
                                         std::end(users),
                                         [=](const toggl::ReportUser &user) {
                                             return user.get<int>(toggl::ReportUser::ID)
                                                    == entry.get<int>(toggl::TimeEntry::USER_ID);
                                         });
        QString key;
        if (userIt != std::end(users)) {
            key = getGroupNaming(userIt->get<QString>(toggl::ReportUser::FULLNAME).value_or(""));
        }
        if (!splitted.contains(key)) {
            splitted.insert(key, {});
        }
        splitted[key].append(entry);
    }
    return splitted;
}

QMap<QString, toggl::TimeEntryList> BillingSummarizer::splitByRule(
    const toggl::TimeEntryList &timeEntryList,
    const QList<toggl::Task> &tasks,
    const QList<toggl::ReportUser> &users) const
{
    QMap<QString, toggl::TimeEntryList> splitted;
    for (const auto &entry : timeEntryList) {
        for (int i = 0; i < _customSplitByRules.count(); ++i) {
            const QString key = getGroupNaming(QObject::tr("Group %1").arg(i + 1));
            const auto rule = _customSplitByRules.value(i);
            if (!splitted.contains(key)) {
                splitted.insert(key, {});
            }
            switch (rule.ruleColumn) {
            case SplitByRule::Column::DESCRIPTION: {
                switch (rule.ruleOperator) {
                case SplitByRule::Operator::EQUALS: {
                    if (entry.get<QString>(toggl::TimeEntry::DESCRIPTION).value_or("")
                        == rule.ruleComparisonValue) {
                        splitted[key].append(entry);
                    }
                    break;
                }
                case SplitByRule::Operator::NOT_EQUALS: {
                    if (entry.get<QString>(toggl::TimeEntry::DESCRIPTION).value_or("")
                        != rule.ruleComparisonValue) {
                        splitted[key].append(entry);
                    }
                    break;
                }
                }
                break;
            }
            case SplitByRule::Column::TASK: {
                auto it = std::find_if(std::begin(tasks),
                                       std::end(tasks),
                                       [=](const toggl::Task &task) {
                                           return task.get<QString>(toggl::Task::NAME).value_or("")
                                                  == rule.ruleComparisonValue;
                                       });
                if (it != std::end(tasks)) {
                    auto taskId = it->get<int>(toggl::Task::ID).value_or(-1);
                    switch (rule.ruleOperator) {
                    case SplitByRule::Operator::EQUALS: {
                        if (entry.get<int>(toggl::TimeEntry::TASK_ID).value_or(-1) == taskId) {
                            splitted[key].append(entry);
                        }
                        break;
                    }
                    case SplitByRule::Operator::NOT_EQUALS: {
                        if (entry.get<int>(toggl::TimeEntry::TASK_ID).value_or(-1) != taskId) {
                            splitted[key].append(entry);
                        }
                        break;
                    }
                    }
                }
                break;
            }
            case SplitByRule::Column::USER: {
                auto it = std::find_if(std::begin(users),
                                       std::end(users),
                                       [=](const toggl::ReportUser &user) {
                                           return user.get<QString>(toggl::ReportUser::FULLNAME)
                                                      .value_or("")
                                                  == rule.ruleComparisonValue;
                                       });
                if (it != std::end(users)) {
                    auto userId = it->get<int>(toggl::ReportUser::ID).value_or(-1);
                    switch (rule.ruleOperator) {
                    case SplitByRule::Operator::EQUALS: {
                        if (entry.get<int>(toggl::TimeEntry::USER_ID).value_or(-1) == userId) {
                            splitted[key].append(entry);
                        }
                        break;
                    }
                    case SplitByRule::Operator::NOT_EQUALS: {
                        if (entry.get<int>(toggl::TimeEntry::USER_ID).value_or(-1) != userId) {
                            splitted[key].append(entry);
                        }
                        break;
                    }
                    }
                }
                break;
            }
            }
        }
    }
    return splitted;
}

QList<BillingSummarizer::SummarizedTimeEntry> BillingSummarizer::summarizeByDay(
    const toggl::TimeEntryList &timeEntryList, const QList<toggl::Task> &tasks) const
{
    QMap<QDateTime, QMap<QString, toggl::TimeEntryList>> resultingMap;
    for (const auto &entry : timeEntryList) {
        QDateTime timeFrameKey = entry.get<QDateTime>(toggl::TimeEntry::START).value_or(QDateTime());
        timeFrameKey.setTime(QTime(0, 0));

        QString groupingKey;
        switch (_groupingType) {
        case GroupingType::GROUP_BY_DESCRIPTION: {
            groupingKey = entry.get<QString>(toggl::TimeEntry::DESCRIPTION).value_or("");
            break;
        }
        case GroupingType::GROUP_BY_TASK: {
            auto taskId = entry.get<int>(toggl::TimeEntry::TASK_ID).value_or(-1);
            auto taskIt = std::find_if(std::begin(tasks),
                                       std::end(tasks),
                                       [=](const toggl::Task &task) {
                                           return task.get<int>(toggl::Task::ID) == taskId;
                                       });
            if (taskIt != std::end(tasks)) {
                groupingKey = taskIt->get<QString>(toggl::Task::NAME).value_or("");
            }
            break;
        }
        default:
            break;
        }
        if (!resultingMap.contains(timeFrameKey)) {
            resultingMap.insert(timeFrameKey, {});
        }
        if (!resultingMap[timeFrameKey].contains(groupingKey)) {
            resultingMap[timeFrameKey].insert(groupingKey, {});
        }
        resultingMap[timeFrameKey][groupingKey].append(entry);
    }
    QList<BillingSummarizer::SummarizedTimeEntry> result;
    for (auto it = resultingMap.begin(); it != resultingMap.end(); ++it) {
        for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2) {
            BillingSummarizer::SummarizedTimeEntry summarizedEntry;
            summarizedEntry.timeFrameKey = it.key();
            summarizedEntry.groupingKey = it2.key();
            summarizedEntry.seconds = it2.value().sum();
            summarizedEntry.summarizedEntries = it2.value();
            result.append(summarizedEntry);
        }
    }

    return result;
}

QList<BillingSummarizer::SummarizedTimeEntry> BillingSummarizer::summarizeByMonth(
    const toggl::TimeEntryList &timeEntryList, const QList<toggl::Task> &tasks) const
{
    QMap<QDateTime, QMap<QString, toggl::TimeEntryList>> resultingMap;
    for (const auto &entry : timeEntryList) {
        QDateTime timeFrameKey = entry.get<QDateTime>(toggl::TimeEntry::START).value_or(QDateTime());
        timeFrameKey.setDate(QDate(timeFrameKey.date().year(), timeFrameKey.date().month(), 1));

        QString groupingKey;
        switch (_groupingType) {
        case GroupingType::GROUP_BY_DESCRIPTION: {
            groupingKey = entry.get<QString>(toggl::TimeEntry::DESCRIPTION).value_or("");
            break;
        }
        case GroupingType::GROUP_BY_TASK: {
            auto taskId = entry.get<int>(toggl::TimeEntry::TASK_ID).value_or(-1);
            auto taskIt = std::find_if(std::begin(tasks),
                                       std::end(tasks),
                                       [=](const toggl::Task &task) {
                                           return task.get<int>(toggl::Task::ID) == taskId;
                                       });
            if (taskIt != std::end(tasks)) {
                groupingKey = taskIt->get<QString>(toggl::Task::NAME).value_or("");
            }
            break;
        }
        default:
            break;
        }
        if (!resultingMap.contains(timeFrameKey)) {
            resultingMap.insert(timeFrameKey, {});
        }
        if (!resultingMap[timeFrameKey].contains(groupingKey)) {
            resultingMap[timeFrameKey].insert(groupingKey, {});
        }
        resultingMap[timeFrameKey][groupingKey].append(entry);
    }
    QList<BillingSummarizer::SummarizedTimeEntry> result;
    for (auto it = resultingMap.begin(); it != resultingMap.end(); ++it) {
        for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2) {
            BillingSummarizer::SummarizedTimeEntry summarizedEntry;
            summarizedEntry.timeFrameKey = it.key();
            summarizedEntry.groupingKey = it2.key();
            summarizedEntry.seconds = it2.value().sum();
            summarizedEntry.summarizedEntries = it2.value();
            result.append(summarizedEntry);
        }
    }
    return result;
}

QList<BillingSummarizer::SummarizedTimeEntry> BillingSummarizer::summarizeByTyoe(
    const toggl::TimeEntryList &timeEntryList, const QList<toggl::Task> &tasks) const
{
    QMap<QDateTime, QMap<QString, toggl::TimeEntryList>> resultingMap;
    for (const auto &entry : timeEntryList) {
        QDateTime timeFrameKey = entry.get<QDateTime>(toggl::TimeEntry::START).value_or(QDateTime());
        QString groupingKey;
        switch (_groupingType) {
        case GroupingType::GROUP_BY_DESCRIPTION: {
            groupingKey = entry.get<QString>(toggl::TimeEntry::DESCRIPTION).value_or("");
            break;
        }
        case GroupingType::GROUP_BY_TASK: {
            auto taskId = entry.get<int>(toggl::TimeEntry::TASK_ID).value_or(-1);
            auto taskIt = std::find_if(std::begin(tasks),
                                       std::end(tasks),
                                       [=](const toggl::Task &task) {
                                           return task.get<int>(toggl::Task::ID) == taskId;
                                       });
            if (taskIt != std::end(tasks)) {
                groupingKey = taskIt->get<QString>(toggl::Task::NAME).value_or("");
            }
            break;
        }
        default:
            break;
        }
        if (!resultingMap.contains(timeFrameKey)) {
            resultingMap.insert(timeFrameKey, {});
        }
        if (!resultingMap[timeFrameKey].contains(groupingKey)) {
            resultingMap[timeFrameKey].insert(groupingKey, {});
        }
        resultingMap[timeFrameKey][groupingKey].append(entry);
    }
    QList<BillingSummarizer::SummarizedTimeEntry> result;
    for (auto it = resultingMap.begin(); it != resultingMap.end(); ++it) {
        for (auto it2 = it.value().begin(); it2 != it.value().end(); ++it2) {
            BillingSummarizer::SummarizedTimeEntry summarizedEntry;
            summarizedEntry.timeFrameKey = it.key();
            summarizedEntry.groupingKey = it2.key();
            summarizedEntry.seconds = it2.value().sum();
            summarizedEntry.summarizedEntries = it2.value();
            result.append(summarizedEntry);
        }
    }
    return result;
}

QString BillingSummarizer::getGroupNaming(const QString &key) const
{
    auto it = std::find_if(std::begin(_groupNaming[_splitBy]),
                           std::end(_groupNaming[_splitBy]),
                           [=](const QPair<QString, QString> &entry) { return entry.first == key; });
    if (it == std::end(_groupNaming[_splitBy])) {
        return key;
    }
    return it->second;
}

bool BillingSummarizer::useWorkspaceRounding() const
{
    return _useWorkspaceRounding;
}

void BillingSummarizer::setUseWorkspaceRounding(bool newUseWorkspaceRounding)
{
    _useWorkspaceRounding = newUseWorkspaceRounding;
}

toggl::Rounding BillingSummarizer::rounding() const
{
    return _rounding;
}

void BillingSummarizer::setRounding(toggl::Rounding newRounding)
{
    _rounding = newRounding;
}

toggl::RoundingMinute BillingSummarizer::roundingMinute() const
{
    return _roundingMinute;
}

void BillingSummarizer::setRoundingMinute(toggl::RoundingMinute newRoundingMinute)
{
    _roundingMinute = newRoundingMinute;
}
