#include <atomic>
#include <SDL3/SDL.h>

#include "../debug/Logger.hpp"
#include "../file/json/Json.hpp"
#include "sources/boolInput.hpp"
#include "sources/mouseMove.hpp"
#include "sources/scroll.hpp"
#include "InputHandler.hpp"

namespace InputHandler {
    typedef int32_t i32;
    typedef uint32_t u32;
    using std::atomic, std::memory_order_acquire, std::memory_order_release;

    atomic<bool> sendToImGui(true), sendToSimulation(true), cursorDisabled(true);


    SDL_Gamepad* gamepadHandle = nullptr;
    atomic<bool> gamepadStateResetSignal(false);

    void init() noexcept {
        //Get gamepad device
        i32 count = 0;
        SDL_JoystickID* ids = SDL_GetJoysticks(&count);
        if (ids != nullptr && count > 0) {
            gamepadHandle = SDL_OpenGamepad(ids[0]);
            SDL_free(ids);
        }
        //Initialize input sources
        BoolInput::init();
        //Get input binding options //todo: migrate to Settings
        auto _result = Json::getJSON("options.json");
        if (_result.has_value()) {
            const auto& result = _result.value();
            lout << "[InputHandler] Key bindings: " << result.dump(4) << endl;
        }
        else Json::saveJSON("options.json");
        //todo: set stored bindings
    }

    bool getCursorDisabled() noexcept { return cursorDisabled.load(memory_order_acquire); }

    void setCursorDisabled(bool disabled) noexcept {
        auto currentDisabled = cursorDisabled.load(memory_order_acquire);
        if (currentDisabled && !disabled) {
            cursorDisabled.store(false, memory_order_release);
        }
        else if (!currentDisabled && disabled) {
            cursorDisabled.store(true, memory_order_release);
        }
    }

    void processTrigger(const SDL_Event& event) noexcept {
        switch(event.type) {
        //Connection events
            case SDL_EVENT_GAMEPAD_ADDED:
                if (gamepadHandle == nullptr) gamepadHandle = SDL_OpenGamepad(event.gdevice.which); 
                break;
            case SDL_EVENT_GAMEPAD_REMOVED:
                if (gamepadHandle != nullptr && event.gdevice.which == SDL_GetGamepadID(gamepadHandle)) {
                    SDL_CloseGamepad(gamepadHandle);
                    gamepadHandle = nullptr;
                    gamepadStateResetSignal = true;
                }
                break;

        //Edge-triggered input events
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                BoolInput::processTrigger(event);
                break;
            case SDL_EVENT_MOUSE_MOTION:
                MouseMove::process(event);
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                Scroll::process(event);
                break;
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                //Stick::process(event);
                break;
            case SDL_EVENT_FINGER_DOWN:
            case SDL_EVENT_FINGER_UP:
            case SDL_EVENT_FINGER_CANCELED:
                lout << "Touch is not supported yet!" << endl;
                break;
            case SDL_EVENT_FINGER_MOTION:
                lout << "Touchmove is not supported yet!" << endl;
                break;
        }
    }

    void processPersist() noexcept {
        BoolInput::processPersist();
    }

    void update() noexcept {
        BoolInput::update();
    }
}