#pragma once
#include <cstdint>

namespace Components {
    typedef uint16_t u16;

    struct LoadingSource {
        //Should be enough (65536 *chunks* in each direction, i.e. 2,097,152 blocks)
        u16 radius;
    };
}