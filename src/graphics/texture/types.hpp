#pragma once

#include "../../util/SlotTable.hpp"

namespace TexturePool {
    MAKE_DISTINCT_HANDLE(TextureAtlasHandle)
    MAKE_DISTINCT_HANDLE(TextureHandle)

    struct UVRect {
        //`u1` and `v1`'s default values should not be `1.0f` in case of attacks that rely on uninitialized memory to leak other packs' texture data.
        float u0{0.0f}, v0{0.0f}, u1{0.0f}, v1{0.0f};
    };
}