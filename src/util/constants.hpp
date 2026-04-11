#pragma once
#include <limits>

namespace Util {
    using std::numeric_limits;

    inline constexpr double
        DOUBLE_INFINITY = numeric_limits<double>::infinity(),
        DOUBLE_NAN = numeric_limits<double>::quiet_NaN();
    inline constexpr float
        FLOAT_INFINITY = numeric_limits<float>::infinity(),
        FLOAT_NAN = numeric_limits<float>::quiet_NaN();
}