#pragma once
#include <cstdint>

#include "types.hpp"

namespace TexturePool::detail {
    typedef uint32_t u32;

    struct TextureInfo {
        //Width corresponds to the U axis and height corresponds to the V axis.
        u32 width, height;
        //All texture coordinates are normalized to the atlas with (u = 0.0f, v = 0.0f) at the top-left and (u = 1.0f, v = 1.0f) at the bottom-right.
        // ------> U axis (width)
        // |
        // |
        // v
        // V axis (height)
        float u0, v0, u1, v1;
        TextureAtlasHandle storageHandle;

        void use() const noexcept;
    };
}