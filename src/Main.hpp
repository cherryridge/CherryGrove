#pragma once
#include <atomic>
#include <latch>

namespace Main {
    using std::atomic, std::latch;

    extern atomic<bool> isCGAlive;
    extern latch subsystemSetupLatch;

    void launch();
}