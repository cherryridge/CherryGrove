#pragma once

#include <bgfx/bgfx.h>

#include "registry.hpp"
#include "uniforms.hpp"

namespace TexturePool::detail {
    inline void TextureInfo::use() const noexcept {
        const auto* atlas = atlasRegistry.get(storageHandle);
        if (atlas == nullptr || !bgfx::isValid(atlas->internalHandle)) return;
        bgfx::setUniform(uvRectUniform, &u0);
        bgfx::setTexture(0, samplerUniform, atlas->internalHandle);
    }
}