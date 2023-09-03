#include "timeentrylist.hpp"

#include <algorithm>

namespace toggl {

    TimeEntryList TimeEntryList::forWorkspace(int workspaceId) const
    {
        TimeEntryList r;
        std::copy_if(begin(), end(), std::back_insert_iterator(r), [=](const TimeEntry& entry) {
            const auto timeEntryWorkspaceId = entry.get<int>(TimeEntry::WORKSPACE_ID).value_or(-1);
            return timeEntryWorkspaceId == workspaceId;
        });
        return r;
    }

    TimeEntryList TimeEntryList::forProject(int projectId) const
    {
        TimeEntryList r;
        std::copy_if(begin(), end(), std::back_insert_iterator(r), [=](const TimeEntry& entry) {
            const auto timeEntryProjectId = entry.get<int>(TimeEntry::PROJECT_ID).value_or(-1);
            return timeEntryProjectId == projectId;
        });
        return r;
    }

    TimeEntryList TimeEntryList::active() const
    {
        TimeEntryList r;
        std::copy_if(begin(), end(), std::back_insert_iterator(r), [](const TimeEntry& entry) {
            const auto deletedAt = entry.get<QDateTime>(TimeEntry::SERVER_DELETED_AT)
                                       .value_or(QDateTime());
            return deletedAt.isNull();
        });
        return r;
    }

    TimeEntryList TimeEntryList::billable() const
    {
        TimeEntryList r;
        std::copy_if(begin(), end(), std::back_insert_iterator(r), [](const TimeEntry& entry) {
            const auto billable = entry.get<bool>(TimeEntry::BILLABLE).value_or(false);
            return billable;
        });
        return r;
    }

    TimeEntryList TimeEntryList::nonBillable() const
    {
        TimeEntryList r;
        std::copy_if(begin(), end(), std::back_insert_iterator(r), [](const TimeEntry& entry) {
            const auto billable = entry.get<bool>(TimeEntry::BILLABLE).value_or(false);
            return !billable;
        });
        return r;
    }

    TimeEntryList TimeEntryList::between(const QDate& afterDate, const QDate& beforeDate) const
    {
        return after(afterDate).before(beforeDate);
    }

    TimeEntryList TimeEntryList::after(const QDate& afterDate) const
    {
        TimeEntryList r;
        std::copy_if(begin(), end(), std::back_inserter(r), [=](const TimeEntry& entry) {
            const auto started_at = entry.get<QDateTime>(TimeEntry::START).value_or(QDateTime());
            return !started_at.isNull() && (started_at.date() >= afterDate);
        });
        return r;
    }

    TimeEntryList TimeEntryList::before(const QDate& beforeDate) const
    {
        TimeEntryList r;
        std::copy_if(begin(), end(), std::back_inserter(r), [=](const TimeEntry& entry) {
            const auto started_at = entry.get<QDateTime>(TimeEntry::START).value_or(QDateTime());
            const auto ended_at = entry.get<QDateTime>(TimeEntry::STOP).value_or(QDateTime());
            if (ended_at.isNull()) {
                return !started_at.isNull() && (started_at.date() <= beforeDate);
            }
            return ended_at.date() <= beforeDate;
        });
        return r;
    }

    TimeEntryList TimeEntryList::today() const
    {
        auto now = QDate::currentDate();
        return after(now).before(now);
    }

    TimeEntryList TimeEntryList::thisWeek() const
    {
        auto now = QDate::currentDate();
        return after(now.addDays(-(now.dayOfWeek() - 1))).before(now);
    }

    TimeEntryList TimeEntryList::thisMonth() const
    {
        auto now = QDate::currentDate();
        auto startOfMonth = QDate(now.year(), now.month(), 1);
        return after(startOfMonth).before(now);
    }

    qint64 TimeEntryList::sum() const
    {
        qint64 r = 0;
        std::for_each(begin(), end(), [&](const TimeEntry& entry) {
            const auto seconds = entry.get<qint64>(TimeEntry::DURATION).value_or(0);
            r += seconds;
        });
        return r;
    }

    template<Rounding round>
    qint64 round_value(std::optional<qint64> value, int roundingMinute)
    {
        if (!value.has_value()) {
            return 0;
        }
        const double roundableValue = static_cast<double>(value.value()) / (60.0 * roundingMinute);

        if (roundableValue < 0) {
            return 0;
        }
        if (round == Rounding::ROUND_DOWN_TO) {
            return qint64(std::floor(roundableValue) * 60 * roundingMinute);
        } else if (round == Rounding::ROUND_TO_NEAREST) {
            return qint64(std::llround(roundableValue) * 60 * roundingMinute);
        } else if (round == Rounding::ROUND_UP_TO) {
            return qint64(std::ceil(roundableValue) * 60 * roundingMinute);
        }
        return 0;
    }

    TimeEntryList TimeEntryList::rounded(Rounding rounding, RoundingMinute roundingMinute) const
    {
        if (rounding == Rounding::NO_ROUNDING || roundingMinute == RoundingMinute::MINUTES_0) {
            return (*this);
        }
        TimeEntryList roundedList;

        switch (rounding) {
        case Rounding::ROUND_DOWN_TO: {
            std::transform(begin(),
                           end(),
                           std::back_insert_iterator(roundedList),
                           [=](const TimeEntry& entry) {
                               auto data = entry.getData();
                               data[TimeEntry::DURATION] = round_value<Rounding::ROUND_DOWN_TO>(
                                   data[TimeEntry::DURATION].toInteger(),
                                   static_cast<int>(roundingMinute));
                               return TimeEntry(data);
                           });
            break;
        }
        case Rounding::ROUND_UP_TO: {
            std::transform(begin(),
                           end(),
                           std::back_insert_iterator(roundedList),
                           [=](const TimeEntry& entry) {
                               auto data = entry.getData();
                               data[TimeEntry::DURATION] = round_value<Rounding::ROUND_UP_TO>(
                                   data[TimeEntry::DURATION].toInteger(),
                                   static_cast<int>(roundingMinute));
                               return TimeEntry(data);
                           });
            break;
        }
        case Rounding::ROUND_TO_NEAREST: {
            std::transform(begin(),
                           end(),
                           std::back_insert_iterator(roundedList),
                           [=](const TimeEntry& entry) {
                               auto data = entry.getData();
                               data[TimeEntry::DURATION] = round_value<Rounding::ROUND_TO_NEAREST>(
                                   data[TimeEntry::DURATION].toInteger(),
                                   static_cast<int>(roundingMinute));
                               return TimeEntry(data);
                           });
            break;
        }
        default:
            break;
        }
        return roundedList;
    }

    qint64 TimeEntryList::sum(Rounding rounding, RoundingMinute roundingMinute) const
    {
        if (rounding == Rounding::NO_ROUNDING || roundingMinute == RoundingMinute::MINUTES_0) {
            return sum();
        }
        qint64 r = 0;
        switch (rounding) {
        case Rounding::ROUND_DOWN_TO: {
            std::for_each(begin(), end(), [&](const TimeEntry& entry) {
                r += round_value<Rounding::ROUND_DOWN_TO>(entry.get<qint64>(TimeEntry::DURATION),
                                                          static_cast<int>(roundingMinute));
            });
            break;
        }
        case Rounding::ROUND_UP_TO: {
            std::for_each(begin(), end(), [&](const TimeEntry& entry) {
                r += round_value<Rounding::ROUND_UP_TO>(entry.get<qint64>(TimeEntry::DURATION),
                                                        static_cast<int>(roundingMinute));
            });
            break;
        }
        case Rounding::ROUND_TO_NEAREST: {
            std::for_each(begin(), end(), [&](const TimeEntry& entry) {
                r += round_value<Rounding::ROUND_TO_NEAREST>(entry.get<qint64>(TimeEntry::DURATION),
                                                             static_cast<int>(roundingMinute));
            });
            break;
        }
        default:
            break;
        }
        return r;
    }
} // namespace toggl
