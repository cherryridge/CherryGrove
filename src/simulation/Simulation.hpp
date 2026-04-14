#pragma once
#include <atomic>

namespace Simulation {
    using std::atomic;

    extern atomic<bool> gameStarted, gamePaused;
    extern atomic<float> perf_MSPT;

    void start() noexcept;
    void exit() noexcept;
}