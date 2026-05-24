#pragma once
#include <atomic>
#include <thread>

namespace Simulation {
    using std::atomic, std::memory_order_acquire, std::memory_order_release, std::thread;

    namespace internal {
        inline atomic<bool> simStarted, simPaused;

        enum struct SimSignal {
            None, Start, Pause, Resume, Exit
        };

        inline atomic<SimSignal> signal{SimSignal::None}; 
    }

    inline void signalStart() noexcept {
        internal::signal.store(internal::SimSignal::Start, memory_order_release);
    }

    inline void signalPause() noexcept {
        internal::signal.store(internal::SimSignal::Pause, memory_order_release);
    }

    inline void signalResume() noexcept {
        internal::signal.store(internal::SimSignal::Resume, memory_order_release);
    }

    inline void signalExit() noexcept {
        internal::signal.store(internal::SimSignal::Exit, memory_order_release);
    }

    [[nodiscard]] inline bool isSimStarted() noexcept {
        return internal::simStarted.load(memory_order_acquire);
    }

    [[nodiscard]] inline bool isSimPaused() noexcept {
        return internal::simPaused.load(memory_order_acquire);
    }
}