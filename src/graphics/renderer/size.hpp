#pragma once
#include <cstdint>
#include <bgfx/bgfx.h>

#include "../../settings/Settings.hpp"
#include "../../window.hpp"

namespace Renderer {
    typedef int32_t i32;

    namespace internal {
        struct WindowInfoCache {
            i32 width{0}, height{0};
            float aspectRatio{1.0f};
        };
    
        inline WindowInfoCache windowInfoCache;
    }

    inline void respondToResize() noexcept {
        i32 newWidth, newHeight;
        SDL_GetWindowSize(Window::getMainWindow(), &newWidth, &newHeight);
        if (newWidth == internal::windowInfoCache.width && newHeight == internal::windowInfoCache.height) return;
        internal::windowInfoCache.width = newWidth;
        internal::windowInfoCache.height = newHeight;
        internal::windowInfoCache.aspectRatio = SDL_GetWindowDisplayScale(Window::getMainWindow());
        bgfx::reset(internal::windowInfoCache.width, internal::windowInfoCache.height, Settings::getSettings().graphics.vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
    }
}