#pragma once
#include <atomic>
#include <SDL3/SDL.h>

namespace InputHandler {
    using std::atomic;

    extern atomic<bool> sendToImGui;
    extern atomic<bool> sendToSimulation;

    extern SDL_Gamepad* gamepadHandle;
    extern atomic<bool> gamepadStateResetSignal;

    void init() noexcept;

    void processTrigger(const SDL_Event& event) noexcept;
    void processPersist() noexcept;
    void update() noexcept;
}