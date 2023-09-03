#ifndef BILLINGSUMMARIZER_HPP
#define BILLINGSUMMARIZER_HPP

#include <QJsonObject>
#include <QList>
#include <QSharedPointer>

class GlobalStorage;

#include "toggl/apiconstants.hpp"
#include "toggl/timeentrylist.hpp"

class BillingSummarizer
{
public:
    struct SummarizedTimeEntry
    {
        enum class TimeFrameKeyDisplay { DayOnly = 0, MonthOnly = 1 };

        QDateTime timeFrameKey;
        inline QString timeFrameKeyFormatted() const
        {
            return timeFrameKeyDisplay == TimeFrameKeyDisplay::DayOnly
                       ? timeFrameKey.date().toString()
                       : timeFrameKey.toString("yyyy-MM");
        }
        TimeFrameKeyDisplay timeFrameKeyDisplay = TimeFrameKeyDisplay::DayOnly;
        QString groupingKey;
        qint64 seconds;
        toggl::TimeEntryList summarizedEntries;
        inline QString format() const
        {
            int _seconds = seconds;
            int hours = _seconds / 3600;
            _seconds = _seconds % 3600;
            int minutes = _seconds / 60;
            _seconds = _seconds % 60;
            return QString("%1:%2:%3")
                .arg(hours, ((hours >= 100) ? 0 : 2), 10, QChar('0'))
                .arg(minutes, 2, 10, QChar('0'))
                .arg(_seconds, 2, 10, QChar('0'));
        }
        inline QString formatDecimal() const
        {
            double hours = static_cast<double>(seconds) / 3600;
            return QString::number(hours, 'f', 4);
        }
    };

    using Result = QMap<QString, QList<SummarizedTimeEntry>>;

    enum class GroupingType { GROUP_BY_TASK = 0, GROUP_BY_DESCRIPTION = 1 };

    enum class GroupingTimeFrame { DO_NOT_GROUP = 0, DAY = 1, MONTH = 2 };

    enum class SplitBy {
        DO_NOT_SPLIT = 0,
        SPLIT_BY_TASK = 1,
        SPLIT_BY_USER = 2,
        SPLIT_BY_RULE = 3
    };

    struct SplitByRule
    {
        enum class Column { USER = 0, TASK = 1, DESCRIPTION = 2 };
        enum class Operator { EQUALS = 0, NOT_EQUALS = 1 };
        Column ruleColumn = Column::USER;
        Operator ruleOperator = Operator::EQUALS;
        QString ruleComparisonValue;
    };

    BillingSummarizer();

    void fromJson(const QJsonObject& object);
    QJsonObject toJson() const;

    void setGroupingType(GroupingType newGroupingType);
    GroupingType groupingType() const;

    GroupingTimeFrame groupingTimeFrame() const;
    void setGroupingTimeFrame(GroupingTimeFrame newGroupingTimeFrame);

    SplitBy splitBy() const;
    void setSplitBy(SplitBy newSplitBy);

    QList<SplitByRule> customSplitByRules() const;
    void setCustomSplitByRules(const QList<SplitByRule>& newCustomSplitByRules);

    QMap<SplitBy, QList<QPair<QString, QString>>> groupNaming() const;
    void setGroupNaming(QMap<SplitBy, QList<QPair<QString, QString>>>& newGroupNaming);

    bool useWorkspaceRounding() const;
    void setUseWorkspaceRounding(bool newUseWorkspaceRounding);

    toggl::Rounding rounding() const;
    void setRounding(toggl::Rounding newRounding);

    toggl::RoundingMinute roundingMinute() const;
    void setRoundingMinute(toggl::RoundingMinute newRoundingMinute);

    Result summarize(const toggl::TimeEntryList& timeEntryList,
                     const QList<toggl::Project>& projects,
                     const QList<toggl::Task>& tasks,
                     const QList<toggl::ReportUser>& users,
                     toggl::Rounding workspaceRounding,
                     toggl::RoundingMinute workspaceRoundingMinute) const;
    QMap<QString, toggl::TimeEntryList> splitByTask(const toggl::TimeEntryList& timeEntryList,
                                                    const QList<toggl::Task>& tasks) const;
    QMap<QString, toggl::TimeEntryList> splitByUser(const toggl::TimeEntryList& timeEntryList,
                                                    const QList<toggl::ReportUser>& users) const;
    QMap<QString, toggl::TimeEntryList> splitByRule(const toggl::TimeEntryList& timeEntryList,
                                                    const QList<toggl::Task>& tasks,
                                                    const QList<toggl::ReportUser>& users) const;

    QList<SummarizedTimeEntry> summarizeByDay(const toggl::TimeEntryList& timeEntryList,
                                              const QList<toggl::Task>& tasks) const;
    QList<SummarizedTimeEntry> summarizeByMonth(const toggl::TimeEntryList& timeEntryList,
                                                const QList<toggl::Task>& tasks) const;
    QList<SummarizedTimeEntry> summarizeByTyoe(const toggl::TimeEntryList& timeEntryList,
                                               const QList<toggl::Task>& tasks) const;

signals:

private:
    QString getGroupNaming(const QString& key) const;

    GroupingType _groupingType = GroupingType::GROUP_BY_TASK;
    GroupingTimeFrame _groupingTimeFrame = GroupingTimeFrame::DO_NOT_GROUP;
    SplitBy _splitBy = SplitBy::DO_NOT_SPLIT;
    QList<SplitByRule> _customSplitByRules = {};
    QMap<SplitBy, QList<QPair<QString, QString>>> _groupNaming;
    bool _useWorkspaceRounding = true;
    toggl::Rounding _rounding = toggl::Rounding::NO_ROUNDING;
    toggl::RoundingMinute _roundingMinute = toggl::RoundingMinute::MINUTES_0;
};

Q_DECLARE_METATYPE(BillingSummarizer);
Q_DECLARE_METATYPE(BillingSummarizer::SummarizedTimeEntry)
#endif // BILLINGSUMMARIZER_HPP
