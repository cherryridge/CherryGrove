#include <memory>
#include <vector>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"
#include "gamepad.hpp"

namespace InputHandler::Gamepad {
    typedef int32_t i32;
    using std::unique_ptr, std::vector;

    static vector<unique_ptr<SDL_Gamepad, SDLGamepadDeleter>> gamepads;

    void init() noexcept {
        i32 count = 0;
        SDL_JoystickID* ids = SDL_GetJoysticks(&count);
        if (ids == nullptr || count == 0) return;
        for (i32 i = 0; i < count; i++) gamepads.emplace_back(SDL_OpenGamepad(ids[i]));
        SDL_free(ids);
        ids = nullptr;
    }

    void shutdown() noexcept { gamepads.clear(); }

    void processDevice(const SDL_Event& event) noexcept {
        switch (event.type) {
            case SDL_EVENT_GAMEPAD_ADDED: {
                SDL_Gamepad* newGamepad = SDL_OpenGamepad(event.gdevice.which);
                if (newGamepad != nullptr) gamepads.emplace_back(newGamepad);
                break;
            }
            case SDL_EVENT_GAMEPAD_REMOVED:
                for (auto it = gamepads.begin(); it != gamepads.end(); ++it) if(SDL_GetGamepadID(it->get()) == event.gdevice.which) {
                    gamepads.erase(it);
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