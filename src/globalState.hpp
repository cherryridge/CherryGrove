#pragma once
#include <atomic>

namespace GlobalState {
    using std::atomic, std::memory_order_acquire, std::memory_order_release;

    namespace detail {
        //This is the most global of global states. It is used to determine whether the program should exit or not.
        inline atomic<bool> isCGAlive{false};

        //This is mainly used to control Logger's behavior.
        inline atomic<bool> multiThreadEra{false};
    }

    [[nodiscard]] inline bool isCGAlive() noexcept {
        return detail::isCGAlive.load(memory_order_acquire);
    }

    inline void setIsCGAlive(bool value) noexcept {
        detail::isCGAlive.store(value, memory_order_release);
    }

    [[nodiscard]] inline bool multiThreadEra() noexcept {
        return detail::multiThreadEra.load(memory_order_acquire);
    }

    inline void setMultiThreadEra(bool value) noexcept {
        return detail::multiThreadEra.store(value, memory_order_release);
    }
}