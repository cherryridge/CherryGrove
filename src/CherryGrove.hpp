#pragma once
#include <atomic>
#include <latch>

namespace CherryGrove {
    using std::atomic, std::latch;

    extern atomic<bool> isCGAlive;
    extern latch subsystemLatch;

    void launch();
}