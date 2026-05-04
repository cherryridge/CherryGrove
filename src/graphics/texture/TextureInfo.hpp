#pragma once
#include <cstdint>
#include <bgfx/bgfx.h>

#include "types.hpp"
#include "uniforms.hpp"

namespace TexturePool::internal {
    typedef uint32_t u32;

    struct TextureInfo {
        //Width corresponds to the U axis and height corresponds to the V axis.
        u32 width{0}, height{0};
        //All texture coordinates are normalized to the atlas with (u = 0.0f, v = 0.0f) at the top-left and (u = 1.0f, v = 1.0f) at the bottom-right.
        // ------> U axis (width)
        // |
        // |
        // v
        // V axis (height)
        UVRect uvRect;
        bgfx::TextureHandle bgfxTextureHandle = BGFX_INVALID_HANDLE;

        void use() const noexcept {
            bgfx::setUniform(uvRectUniform, &uvRect);
            bgfx::setTexture(0, samplerUniform, bgfxTextureHandle);
        }
    };
}