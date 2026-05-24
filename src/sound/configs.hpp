#pragma once
#include <cstdint>

namespace Sound::detail {
    typedef uint32_t u32;
    
    inline constexpr u32 MAX_DEQUEUE_PER_LOOP = 16, MAX_GC_PER_LOOP = 64;
}