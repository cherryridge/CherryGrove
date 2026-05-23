#pragma once
#include <atomic>
#include <thread>

namespace Simulation {
    using std::atomic, std::memory_order_acquire, std::memory_order_release, std::thread;

    namespace detail {
        inline thread simulationThread;

        inline atomic<bool> simStarted, simPaused;

        enum struct SimSignal {
            None, Start, Pause, Resume, Exit
        };

        inline atomic<SimSignal> signal{SimSignal::None}; 
    }

    inline void signalStart() noexcept {
        detail::signal.store(detail::SimSignal::Start, memory_order_release);
    }

    inline void signalPause() noexcept {
        detail::signal.store(detail::SimSignal::Pause, memory_order_release);
    }

    inline void signalResume() noexcept {
        detail::signal.store(detail::SimSignal::Resume, memory_order_release);
    }

    inline void signalExit() noexcept {
        detail::signal.store(detail::SimSignal::Exit, memory_order_release);
    }

    [[nodiscard]] inline bool isSimStarted() noexcept {
        return detail::simStarted.load(memory_order_acquire);
    }

    [[nodiscard]] inline bool isSimPaused() noexcept {
        return detail::simPaused.load(memory_order_acquire);
    }
}