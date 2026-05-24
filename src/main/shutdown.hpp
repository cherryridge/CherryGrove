#pragma once
#include <atomic>
#include <SDL3/SDL.h>

#include "../debug/controller.hpp"
#include "../graphics/controller.hpp"
#include "../simulation/controller.hpp"
#include "../sound/controller.hpp"
#include "../window.hpp"

namespace Main {
    using std::memory_order_acquire;

    inline void shutdown() noexcept {
        Graphics::shutdown();
        Sound::shutdown();
        Simulation::shutdownThread();
        Window::shutdown();
        Debug::shutdown();
    }
}