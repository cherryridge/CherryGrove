#pragma once
#include <imgui.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "debug/Fatal.hpp"
#include "debug/Logger.hpp"
#include "util/os/platform.hpp"

namespace Window {

    namespace detail {
        inline SDL_Window* mainWindow{nullptr};
    }
    
    //note: Pre multithread era.
    inline void initMainWindow(const char* title, int width, int height, const char* iconPath) noexcept {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
            lerr << "[Window] Failed to set up SDL!" << endl;
            Fatal::exit(Fatal::SDL_INITIALIZATION_FAILED);
        }
        detail::mainWindow = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_HIGH_PIXEL_DENSITY);
        if (detail::mainWindow == nullptr) {
            lerr << "[Window] Failed to create main window: " << SDL_GetError() << endl;
            SDL_Quit();
            Fatal::exit(Fatal::SDL_CREATE_WINDOW_FAILED);
        }
        if (iconPath != nullptr) {
            auto* icon = IMG_Load(iconPath);
            if (icon != nullptr) SDL_SetWindowIcon(detail::mainWindow, icon);
            else lerr << "[Window] Failed to load icon from " << iconPath << "!" << endl;
            SDL_DestroySurface(icon);
        }
    }

    inline void shutdown() noexcept {
        SDL_DestroyWindow(detail::mainWindow);
        SDL_Quit();
    }

    [[nodiscard]] inline SDL_Window* getMainWindow() noexcept { return detail::mainWindow; }

    [[nodiscard]] inline bool isLinuxWayland() noexcept {
    #if CG_PLATFORM_LINUX
        if (detail::mainWindow == nullptr) return false;
        const auto properties = SDL_GetWindowProperties(detail::mainWindow);
        void* waylandDisplay = SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
        if (waylandDisplay != nullptr) return true;
        void* waylandSurface = SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
        return waylandSurface != nullptr;
    #else
        static_cast<void>(detail::mainWindow);
        return false;
    #endif
    }

    [[nodiscard]] inline void* getPlatformHandle(SDL_Window* window) noexcept {
        const auto properties = SDL_GetWindowProperties(window);
    #if CG_PLATFORM_WINDOWS
        return SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    #elif CG_PLATFORM_LINUX
        if (isLinuxWayland()) return SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
        else return reinterpret_cast<void*>(static_cast<uintptr_t>(SDL_GetNumberProperty(properties, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0)));
    #elif CG_PLATFORM_MACOS
        return SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
    #elif CG_PLATFORM_ANDROID
        Fatal::exit(Fatal::MISC_UNSUPPORTED_PLATFORM);
        return SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, nullptr);
    #elif CG_PLATFORM_IOS
        Fatal::exit(Fatal::MISC_UNSUPPORTED_PLATFORM);
        return SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
    #endif
    }

    [[nodiscard]] inline void* getNativeWindowHandle(const ImGuiViewport& viewport) noexcept {
        const auto windowId = static_cast<SDL_WindowID>(
        reinterpret_cast<uintptr_t>(viewport.PlatformHandle));
        SDL_Window* window = SDL_GetWindowFromID(windowId);
        if (window == nullptr) {
            lerr << "[Window] Failed to get SDL_Window from viewport: " << SDL_GetError() << endl;
            return nullptr;
        }
        return getPlatformHandle(window);
    }
}