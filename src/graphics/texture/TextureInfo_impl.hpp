#pragma once

#include <bgfx/bgfx.h>

#include "TextureInfo.hpp"
#include "uniforms.hpp"

namespace TexturePool::detail {
    inline void TextureInfo::use() const noexcept {
        bgfx::setUniform(uvRectUniform, &uvRect);
        bgfx::setTexture(0, samplerUniform, bgfxTextureHandle);
    }
}