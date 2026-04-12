#include <atomic>
#include <chrono>
#include <boost/unordered/unordered_flat_map.hpp>
#include <SDL3/SDL.h>

#include "../debug/Logger.hpp"
#include "../settings/Settings.hpp"
#include "../util/concurrentQueue.hpp"
#include "../window.hpp"
#include "Action.hpp"
#include "boolInput/boolInput.hpp"
#include "gamepad/gamepad.hpp"
#include "InputKind.hpp"
#include "mouseMove/mouseMove.hpp"
#include "scroll/scroll.hpp"
#include "stick/stick.hpp"

#include "InputHandler.hpp"

namespace InputHandler {
    typedef int16_t i16;
    using std::atomic, std::chrono::milliseconds, std::memory_order_acquire, std::memory_order_release, boost::unordered_flat_map, Util::SPSCQueue;

    atomic<bool> sendToImGui{true}, sendToSimulation{true};

//#region: Lifecycle

    //threaded: Main Thread
    void init() noexcept {
        Gamepad::init();
        const auto& settings = Settings::getSettings();
        BoolInput::updateArguments(milliseconds(settings.input.repeatTapGap), milliseconds(settings.input.comboMinTTL));
        Stick::updateArguments(settings.input.deadzones);
        //todo: query settings for bindings.
    }

    //threaded: Main Thread
    void shutdown() noexcept {
        Gamepad::shutdown();
    }

    static atomic<bool> canDelete{true};

    [[nodiscard]] bool getCanDelete() noexcept { return canDelete.load(memory_order_acquire); }

    void setCanDelete(bool canDelete) noexcept { InputHandler::canDelete.store(canDelete, memory_order_release); }

//#endregion

//#region: Action ID management

    static unordered_flat_map<ActionID, ActionLocation> actionIdToKind;
    static ActionID nextId{1};

    [[nodiscard]] ActionID internal::getNextId() noexcept {
        const auto id = nextId;
        nextId++;
        return id;
    }

    void internal::registerId(ActionID id, ActionLocation al) noexcept { actionIdToKind.emplace(id, al); }
    void internal::unregisterId(ActionID id) noexcept { actionIdToKind.erase(id); }

    [[nodiscard]] bool getLocation(ActionID id, ActionLocation& result, InputKind expectedKind) noexcept {
        const auto it = actionIdToKind.find(id);
        if (it == actionIdToKind.end()) return false;
        else {
            result = it->second;
            return result.kind == expectedKind;
        }
    }

//#endregion

//#region: Pointer Lock

    static atomic<bool> pointerLocked{true};

    [[nodiscard]] bool getPointerLocked() noexcept { return pointerLocked.load(memory_order_acquire); }

    void setPointerLocked(bool locked) noexcept {
        pointerLocked.store(locked, memory_order_release);
        SDL_SetWindowRelativeMouseMode(Window::getMainWindow(), locked);
    }

//#endregion

//#region: Input Processing

    static SPSCQueue<SDL_Event> eventQueue;

    //threaded: Main thread
    void submitEvent(const SDL_Event& event) noexcept { eventQueue.enqueue(event); }

    //threaded: Simulation thread
    //Router for different event types.
    void processTrigger() noexcept {
        SDL_Event event;
        u64 i = 0;
        for (; i < MAXIMUM_INPUT_EVENTS_PER_FRAME; i++) {
            if (!eventQueue.dequeue(event)) break;
            switch(event.type) {
            //Device events
                case SDL_EVENT_GAMEPAD_ADDED:
                case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
                    Gamepad::processDevice(event);
                    break;
                case SDL_EVENT_GAMEPAD_REMOVED:
                    Gamepad::processDevice(event);
                    Stick::processDevice(event);
                    [[fallthrough]];
                case SDL_EVENT_KEYBOARD_REMOVED:
                case SDL_EVENT_MOUSE_REMOVED:
                    BoolInput::processDevice(event);
                    break;
            //Input events
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                case SDL_EVENT_GAMEPAD_BUTTON_UP:
                    BoolInput::processTrigger(event);
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    MouseMove::processTrigger(event);
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    Scroll::processTrigger(event);
                    break;
                case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                    Stick::processTrigger(event);
                    break;
                case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
                case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
                case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
                #if CG_DEBUG
                    lout << "Gamepad touch is not supported yet!" << endl;
                #endif
                    break;
                case SDL_EVENT_FINGER_DOWN:
                case SDL_EVENT_FINGER_UP:
                case SDL_EVENT_FINGER_CANCELED:
                case SDL_EVENT_FINGER_MOTION:
                #if CG_DEBUG
                    lout << "Touch is not supported yet!" << endl;
                #endif
                    break;
                case SDL_EVENT_PEN_DOWN:
                case SDL_EVENT_PEN_UP:
                case SDL_EVENT_PEN_MOTION:
                case SDL_EVENT_PEN_AXIS:
                #if CG_DEBUG
                    lout << "Pen is not supported yet!" << endl;
                #endif
                    break;
                default:
                #if CG_DEBUG
                    lout << "Got this event type, fyi: " << event.type << endl;
                #endif
                    break;
            }
        }
    #if CG_DEBUG
        if (i == MAXIMUM_INPUT_EVENTS_PER_FRAME) lout << "[InputHandler] Maximum events per frame reached: " << MAXIMUM_INPUT_EVENTS_PER_FRAME << endl;
    #endif
    }

    //threaded: Simulation thread
    void processPersist() noexcept {
        BoolInput::processPersist();
        MouseMove::processPersist();
        Stick::processPersist();
    }

//#endregion
}