#pragma once
#include <atomic>
#include <thread>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"
#include "../../window.hpp"
#include "size.hpp"

namespace Renderer {
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::array, std::thread, std::atomic, std::memory_order_acquire, std::memory_order_release;

    inline bgfx::VertexBufferHandle vertexBuffer;
    inline bgfx::VertexLayout vertexLayout;
    inline bgfx::IndexBufferHandle indexBuffer;

    inline void init() noexcept {
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
        vertexLayout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
        respondToResize();
    }

    inline void shutdown() noexcept { bgfx::shutdown(); }
}