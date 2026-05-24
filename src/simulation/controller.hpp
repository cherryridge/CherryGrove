#pragma once
#include <atomic>
#include <thread>

#include "../debug/loggers.hpp"
#include "../input/InputHandler.hpp"
#include "../pack/Pack.hpp"
#include "hold.hpp"
#include "states.hpp"

namespace Simulation {
    using std::atomic, std::thread, std::memory_order_acquire, std::memory_order_release;

    namespace internal {
        inline thread simulationThread;
        inline atomic<bool> initialized{false};
    }

    namespace detail {
        inline void shutdown() noexcept {
            internal::initialized.store(false, memory_order_release);
            Pack::shutdown();
            InputHandler::shutdown();
            lout << "Simulation thread terminated!" << nlaf;
        }

        inline void init() noexcept {
            Debug::setThreadName("Simulation");
            lout << "Hello from simulation thread!" << nlaf;
            InputHandler::init();
            Pack::init();
            internal::initialized.store(true, memory_order_release);
            internal::hold();
            shutdown();
        }
    }

    inline void initThread() noexcept {
        internal::simulationThread = thread(detail::init);
    }

    [[nodiscard]] inline bool isInitialized() noexcept {
        return internal::initialized.load(memory_order_acquire);
    }

    inline void shutdownThread() noexcept {
        if (isSimStarted()) signalExit();
        if (internal::simulationThread.joinable()) internal::simulationThread.join();
    }
}