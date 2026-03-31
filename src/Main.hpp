#pragma once
#include <atomic>
#include <functional>
#include <SDL3/SDL.h>

#include "util/concurrentQueue.hpp"

namespace Main {
    using std::atomic, std::function, Util::MPSCQueue;

    extern atomic<bool> isCGAlive, multiThreadEra;
    extern SDL_Window* windowHandle;
    extern MPSCQueue<function<void()>> runOnMainThreadMQ;
}