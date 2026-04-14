#pragma once
#include <SDL3/SDL.h>

namespace InputHandler::Gamepad {

    //We currently composes every gamepad because CG is designed for 1p1m. If there is a need for multiple gamepads, that means that we're probably implementing some gruesome split-screen mode, and we will run into much bigger problems than input handling, so TL;DR, relax.
    //And yes, we compose stick movements, so players can cancel each other out by tilting in opposite directions. For more information visit gamepad/...hpp.

    struct SDLGamepadDeleter {
        void operator()(SDL_Gamepad* gamepad) const noexcept {
            if (gamepad != nullptr) SDL_CloseGamepad(gamepad);
            gamepad = nullptr;
        }
    };

    void init() noexcept;
    void shutdown() noexcept;

    //threaded: Simulation thread
    void processDevice(const SDL_Event& event) noexcept;
}