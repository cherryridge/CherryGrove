#pragma once
#include <cstdint>

namespace Components {
    typedef uint16_t u16;

    struct CreatesLoadingZone {
        //Should be enough (65536 *chunks* in each direction, i.e. 1048576 blocks)
        u16 radius;
    };
}