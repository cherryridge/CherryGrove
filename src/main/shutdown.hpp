#pragma once
#include <atomic>
#include <SDL3/SDL.h>

#include "../debug/controller.hpp"
#include "../pack/Pack.hpp"
#include "../graphics/controller.hpp"
#include "../input/InputHandler.hpp"
#include "../simulation/Simulation.hpp"
#include "../sound/controller.hpp"
#include "../window.hpp"

namespace Main {
    using std::memory_order_acquire;

    inline void shutdown() noexcept {
        if (Simulation::gameStarted.load(memory_order_acquire)) Simulation::exit();
        Pack::shutdown();
        Graphics::shutdown();
        Sound::shutdown();
        InputHandler::shutdown();
        Window::shutdown();
        Debug::shutdown();
    }
}