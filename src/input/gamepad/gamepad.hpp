#pragma once
#include <memory>
#include <vector>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"

namespace InputHandler::Gamepad {
    typedef int32_t i32;
    using std::unique_ptr, std::vector;

    //We currently composes every gamepad because CG is designed for 1p1m. If there is a need for multiple gamepads, that means that we're probably implementing some gruesome split-screen mode, and we will run into much bigger problems than input handling, so TL;DR, relax.
    //And yes, we compose stick movements, so players can cancel each other out by tilting in opposite directions.

    namespace detail {
        struct SDLGamepadDeleter {
            void operator()(SDL_Gamepad* gamepad) const noexcept {
                if (gamepad != nullptr) SDL_CloseGamepad(gamepad);
                gamepad = nullptr;
            }
        };

        inline vector<unique_ptr<SDL_Gamepad, SDLGamepadDeleter>> gamepads;
    }

    inline void init() noexcept {
        i32 count = 0;
        SDL_JoystickID* ids = SDL_GetJoysticks(&count);
        if (ids == nullptr || count == 0) return;
        for (i32 i = 0; i < count; i++) detail::gamepads.emplace_back(SDL_OpenGamepad(ids[i]));
        SDL_free(ids);
        ids = nullptr;
    }

    inline void shutdown() noexcept { detail::gamepads.clear(); }

    //threaded: Simulation thread
    inline void processDevice(const SDL_Event& event) noexcept {
        switch (event.type) {
            case SDL_EVENT_GAMEPAD_ADDED: {
                SDL_Gamepad* newGamepad = SDL_OpenGamepad(event.gdevice.which);
                if (newGamepad != nullptr) detail::gamepads.emplace_back(newGamepad);
                break;
            }
            case SDL_EVENT_GAMEPAD_REMOVED:
                for (auto it = detail::gamepads.begin(); it != detail::gamepads.end(); ++it) if(SDL_GetGamepadID(it->get()) == event.gdevice.which) {
                    detail::gamepads.erase(it);
                    break;
                }
                break;
            //This device event does not have a gamepad conterpart and we probably need to use it later.
            case SDL_EVENT_JOYSTICK_BATTERY_UPDATED: break;
            default:
            #if CG_DEBUG
                lerr << "[InputHandler] This event type should not be routed to Gamepad::processDevice: " << event.type << endl;
            #endif
        }
    }
}