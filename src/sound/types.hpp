#pragma once
#include <limits>

#include "../util/SlotTable.hpp"

namespace Sound {
    using Util::GenerationalHandle, std::numeric_limits;

    struct SoundHandle {
        GenerationalHandle value;
    };

    struct PlayHandle {
        GenerationalHandle value;
    };

    inline constexpr float FLOAT_INFINITY = numeric_limits<float>::max();
}