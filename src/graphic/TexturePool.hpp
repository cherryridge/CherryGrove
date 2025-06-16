﻿#pragma once
#include <cstdint>
#include <bgfx/bgfx.h>
#include <SDL3_image/SDL_image.h>

namespace TexturePool {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef int32_t i32;
    typedef uint32_t TextureID;

    struct Texture {
        bgfx::TextureHandle handle;
        SDL_Surface* data;
    };

    struct TextureInfo {
        i32 height, width;
    };

    inline constexpr TextureID MISSING_TEXTURE_ID = 0;

    void init(const char* samplerName) noexcept;
    void shutdown() noexcept;
    TextureID addTexture(const char* filePath) noexcept;
    void useTexture(TextureID id, u8 textureDataIndex = 0) noexcept;
    bool getTextureInfo(TextureID id, TextureInfo& result) noexcept;
    void removeTexture(TextureID id) noexcept;
}