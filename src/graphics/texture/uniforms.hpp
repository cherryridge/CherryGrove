#pragma once
#include <bgfx/bgfx.h>

#include "../../debug/Fatal.hpp"
#include "../../debug/loggers.hpp"

namespace TexturePool::internal {
    inline bgfx::UniformHandle samplerUniform = BGFX_INVALID_HANDLE;
    inline bgfx::UniformHandle uvRectUniform = BGFX_INVALID_HANDLE;

    inline void initUniforms() noexcept {
        samplerUniform = bgfx::createUniform("s_texture", bgfx::UniformType::Sampler);
        uvRectUniform = bgfx::createUniform("u_uvRect", bgfx::UniformType::Vec4);
        if (!bgfx::isValid(samplerUniform) || !bgfx::isValid(uvRectUniform)) {
            lerr << "[TexturePool] Failed to create uniforms." << nlaf;
            Debug::exit(Debug::TEXTUREPOOL_INITIALIZATION_FAILED);
        }
    }

    inline void destroyUniforms() noexcept {
        if (bgfx::isValid(samplerUniform)) bgfx::destroy(samplerUniform);
        if (bgfx::isValid(uvRectUniform)) bgfx::destroy(uvRectUniform);
        samplerUniform = BGFX_INVALID_HANDLE;
        uvRectUniform = BGFX_INVALID_HANDLE;
    }
}