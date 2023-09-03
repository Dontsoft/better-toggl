#ifndef APICONSTANTS_HPP
#define APICONSTANTS_HPP

#include <QtGlobal>
#include "QtCore/qobjectdefs.h"
#include "QtCore/qtmetamacros.h"

namespace toggl {
    Q_NAMESPACE
    enum class Rounding {
        NO_ROUNDING = 0,
        ROUND_UP_TO = 1,
        ROUND_TO_NEAREST = 2,
        ROUND_DOWN_TO = 3
    };

    enum class RoundingMinute {
        MINUTES_0 = 0,
        MINUTES_1 = 1,
        MINUTES_5 = 5,
        MINUTES_6 = 6,
        MINUTES_10 = 10,
        MINUTES_12 = 12,
        MINUTES_15 = 15,
        MINUTES_30 = 30,
        HOURS_1 = 60,
        HOURS_4 = 240
    };
    Q_ENUM_NS(toggl::Rounding);
    Q_ENUM_NS(toggl::RoundingMinute);
} // namespace toggl

#endif // APICONSTANTS_HPP
