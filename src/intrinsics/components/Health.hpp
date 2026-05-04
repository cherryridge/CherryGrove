#pragma once
#include <cstdint>

namespace Components {
    typedef uint32_t u32;

    struct Health {
        u32 health{0}, maxHealth{0};
    };
}