#pragma once
#include <cstdint>
#include <bgfx/bgfx.h>
#include <SDL3/SDL.h>

#include "../debug/Fatal.hpp" // IWYU pragma: keep
#include "../debug/Logger.hpp"
#include "../window.hpp"
#include "backend/imgui_impl_bgfx.hpp"

namespace Graphics {
    typedef int32_t i32;

    inline void initBgfx() noexcept {
        bgfx::Init config;
        config.platformData.nwh = Window::getPlatformHandle(Window::getMainWindow());
        const auto propertyHandle = SDL_GetWindowProperties(Window::getMainWindow());
    #if CG_PLATFORM_WINDOWS
        config.platformData.ndt = nullptr;
    #elif CG_PLATFORM_LINUX
        const char* sessionType = secure_getenv("XDG_SESSION_TYPE");
        if ((sessionType && strcmp(sessionType, "wayland") == 0) || getenv("WAYLAND_DISPLAY")) {
            config.platformData.ndt = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
            config.platformData.type = bgfx::NativeWindowHandleType::Wayland;
        }
        else{
            config.platformData.ndt = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
            config.platformData.type = bgfx::NativeWindowHandleType::Default;
        }
    #elif CG_PLATFORM_MACOS
        config.platformData.ndt = nullptr;
    #elif CG_PLATFORM_ANDROID
        Fatal::exit(Fatal::MISC_UNSUPPORTED_PLATFORM);
    #elif CG_PLATFORM_IOS
        Fatal::exit(Fatal::MISC_UNSUPPORTED_PLATFORM);
    #endif
        //Let bgfx select the rendering backend automatically.
        config.type = bgfx::RendererType::Count;
        //Let bgfx select the adapter automatically.
        config.vendorId = BGFX_PCI_ID_NONE;
        i32 width, height;
        SDL_GetWindowSize(Window::getMainWindow(), &width, &height);
        config.resolution.width = width;
        config.resolution.height = height;
        config.resolution.reset = BGFX_RESET_VSYNC;
        if (!bgfx::init(config)) {
            lerr << "[Renderer] Failed to initialize bgfx!" << endl;
            Fatal::exit(Fatal::BGFX_INITIALIZATION_FAILED);
        }
        lout << "Using rendering backend: " << bgfx::getRendererName(bgfx::getRendererType()) << endl;
    }

    inline void initImGui() noexcept {

    }

    inline void initRenderer() noexcept {

    }

    inline void initGui() noexcept {

    }

    inline void init() noexcept {
        
    }
}