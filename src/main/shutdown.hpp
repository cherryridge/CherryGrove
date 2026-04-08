#pragma once
#include <atomic>
#include <SDL3/SDL.h>

#include "../pack/Pack.hpp"
#include "../globalState.hpp"
#include "../graphics/renderer/Renderer.hpp"
#include "../simulation/Simulation.hpp"
#include "../sound/Sound.hpp"

namespace Main {
    using std::memory_order_acquire;

    inline void shutdown() noexcept {
        Renderer::shutdown();
        if (Simulation::gameStarted.load(memory_order_acquire)) Simulation::exit();
        SDL_DestroyWindow(GlobalState::windowHandle);
        SDL_Quit();
        Pack::shutdown();
        Sound::shutdown();
        Logger::shutdown();
    }
}