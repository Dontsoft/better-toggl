#ifndef TOGGL_TIMEENTRYLIST_HPP
#define TOGGL_TIMEENTRYLIST_HPP

#include <QList>

#include "apiconstants.hpp"
#include "apitypes.hpp"

namespace toggl {

    class TimeEntryList : public QList<toggl::TimeEntry>
    {
    public:
        using QList<toggl::TimeEntry>::QList;

        TimeEntryList forWorkspace(int workspaceId) const;
        TimeEntryList forProject(int projectId) const;
        TimeEntryList active() const;
        TimeEntryList billable() const;
        TimeEntryList nonBillable() const;
        TimeEntryList between(const QDate& afterDate, const QDate& beforeDate) const;
        TimeEntryList after(const QDate& afterDate) const;
        TimeEntryList before(const QDate& beforeDate) const;

        TimeEntryList today() const;
        TimeEntryList thisWeek() const;
        TimeEntryList thisMonth() const;

        TimeEntryList rounded(Rounding rounding, RoundingMinute roundingMinute) const;

        qint64 sum() const;
        qint64 sum(Rounding rounding, RoundingMinute roundingMinute) const;
    };

} // namespace toggl

Q_DECLARE_METATYPE(toggl::TimeEntryList);

#endif // TOGGL_TIMEENTRYLIST_HPP
