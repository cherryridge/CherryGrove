#pragma once
#include <atomic>
#include <SDL3/SDL.h>

namespace GlobalState {
    using std::atomic;

    //This is the most global of global states. It is used to determine whether the program should exit or not.
    //todo: Wrap in a method so other files don't have to include `<atomic>`.
    inline atomic<bool> isCGAlive{false};

    //This is mainly used to control Logger's behavior.
    inline atomic<bool> multiThreadEra{false};

    inline SDL_Window* windowHandle{nullptr};
}