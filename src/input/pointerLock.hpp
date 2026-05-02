#pragma once
#include <atomic>
#include <SDL3/SDL.h>

#include "../window.hpp"

namespace InputHandler {
    using std::atomic, std::memory_order_acquire, std::memory_order_release;

    namespace detail {
        inline atomic<bool> pointerLocked{true};
    }

    [[nodiscard]] inline bool getPointerLocked() noexcept { return detail::pointerLocked.load(memory_order_acquire); }

    inline void setPointerLocked(bool locked) noexcept {
        detail::pointerLocked.store(locked, memory_order_release);
        SDL_SetWindowRelativeMouseMode(Window::getMainWindow(), locked);
    }
}