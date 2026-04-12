#pragma once
#include <cstdint>
#include <bgfx/bgfx.h>

#include "../../settings/Settings.hpp"
#include "../../window.hpp"

namespace Renderer {
    typedef int32_t i32;

    struct WindowInfoCache {
        i32 width{0}, height{0};
        float aspectRatio{1.0f};
    };
    
    inline WindowInfoCache windowInfoCache;
    
    inline void respondToResize() noexcept {
        i32 newWidth, newHeight;
        SDL_GetWindowSize(Window::getMainWindow(), &newWidth, &newHeight);
        if (newWidth == windowInfoCache.width && newHeight == windowInfoCache.height) return;
        windowInfoCache.width = newWidth;
        windowInfoCache.height = newHeight;
        windowInfoCache.aspectRatio = SDL_GetWindowDisplayScale(Window::getMainWindow());
        bgfx::reset(windowInfoCache.width, windowInfoCache.height, Settings::getSettings().graphics.vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
    }
}