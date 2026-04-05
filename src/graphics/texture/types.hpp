#pragma once
#include <cstdint>

#include "../../util/SlotTable.hpp"

namespace TexturePool {
    typedef uint32_t u32;

    MAKE_DISTINCT_HANDLE(TextureAtlasHandle)
    MAKE_DISTINCT_HANDLE(TextureHandle)

    struct Rect {
        u32 x, y, width, height;
    };

    struct UVRect {
        float u0, v0, u1, v1;
    };
}