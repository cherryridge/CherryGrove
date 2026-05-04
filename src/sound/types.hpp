#pragma once
#include <limits>

#include "../util/SlotTable.hpp"

namespace Sound {
    using Util::GenerationalHandle, std::numeric_limits;

    MAKE_DISTINCT_HANDLE(SoundHandle)
    MAKE_DISTINCT_HANDLE(PlayHandle)

    inline constexpr float FLOAT_INFINITY = numeric_limits<float>::max();
}