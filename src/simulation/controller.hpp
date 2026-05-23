#pragma once
#include <atomic>
#include <thread>

#include "hold.hpp"
#include "states.hpp"

namespace Simulation {
    using std::atomic, std::thread, std::memory_order_acquire, std::memory_order_release;

    inline void initThread() noexcept {
        detail::simulationThread = thread(internal::hold);
    }

    inline void shutdownThread() noexcept {
        if (isSimStarted()) signalExit();
        if (detail::simulationThread.joinable()) detail::simulationThread.join();
    }
}