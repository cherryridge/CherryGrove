#pragma once
#include <cstdint>

namespace TexturePool {
    typedef uint32_t u32;

    struct TextureDescriptor {
        u32 height, width;
        float u0, v0, u1, v1;
    };

    
}