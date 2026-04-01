#pragma once
#include <atomic>
#include <SDL3/SDL.h>

namespace Main {
    using std::atomic;

    inline atomic<bool> isCGAlive{false}, multiThreadEra{false};
    inline SDL_Window* windowHandle{nullptr};
}