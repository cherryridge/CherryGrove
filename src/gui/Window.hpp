#pragma once
#include <functional>
#include <SDL3/SDL.h>

namespace Window {
    typedef uint32_t u32;
    using std::function;

    extern SDL_Window* windowHandle;

    void initSDL(u32 width, u32 height, const char* title) noexcept;
    void update() noexcept;
    void close() noexcept;

    void runOnMainThread(function<void()> callback) noexcept;
}