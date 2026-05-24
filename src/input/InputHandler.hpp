#pragma once
#include <chrono>
#include <SDL3/SDL.h>

#include "../debug/loggers.hpp" // IWYU pragma: keep
#include "../settings/Settings.hpp"
#include "../util/concurrentQueue.hpp"
#include "boolInput/boolInput.hpp"
#include "config.hpp"
#include "fileDrop/fileDrop.hpp"
#include "gamepad/gamepad.hpp"
#include "inputPipeline.hpp"
#include "mouseMove/mouseMove.hpp"
#include "scroll/scroll.hpp"
#include "stick/stick.hpp"

//important: note: threaded: Now every function in the whole InputHandler namespace is called from the Simulation thread. There is no code in this namespace that is called from the main thread, so no need for synchronization primitives here. The main thread interacts by posting tasks to the Simulation thread.

namespace InputHandler {
    typedef uint64_t u64;
    using std::chrono::milliseconds, Util::SPSCQueue;

//#region: Lifecycle

    inline void init() noexcept {
        Gamepad::init();
        const auto& settings = Settings::getSettings();
        BoolInput::updateArguments(milliseconds(settings.input.repeatTapGap), milliseconds(settings.input.comboMinTTL));
        Stick::updateArguments(settings.input.deadzones);
        //todo: query settings for bindings.
    }

    inline void shutdown() noexcept { Gamepad::shutdown(); }

//#endregion

//#region: Input Processing

    namespace detail {
        inline FramedImGuiFlags cachedFlags{};
    }

    //Router for different event types.
    inline void processTrigger() noexcept {
        FramedSDLEvents events;
        {
            FramedImGuiFlags flags;
            while (flagQueue_R2S.dequeue(flags)) detail::cachedFlags = flags;
        }
        const bool blockMouse = detail::cachedFlags.wantCaptureMouse;
        const bool blockKeyboard = detail::cachedFlags.wantCaptureKeyboard;
        u64 i = 0;
        for (; i < MAXIMUM_INPUT_EVENTS_PER_FRAME; i++) {
            if (!inputQueue_M2S.dequeue(events)) break;
            for (u64 j = 0; j < events.actualSize; j++) {
                const SDL_Event& event = events.events[j];
                switch (event.type) {
                //File drop events
                    case SDL_EVENT_DROP_FILE:
                        FileDrop::processTrigger(event);
                        break;
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
                        if (!blockKeyboard) BoolInput::processTrigger(event);
                        break;
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    case SDL_EVENT_MOUSE_BUTTON_UP:
                        if (!blockMouse) BoolInput::processTrigger(event);
                        break;
                    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                    case SDL_EVENT_GAMEPAD_BUTTON_UP:
                        BoolInput::processTrigger(event);
                        break;
                    case SDL_EVENT_MOUSE_MOTION:
                        if (!blockMouse) MouseMove::processTrigger(event);
                        break;
                    case SDL_EVENT_MOUSE_WHEEL:
                        if (!blockMouse) Scroll::processTrigger(event);
                        break;
                    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                        Stick::processTrigger(event);
                        break;
                    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
                    case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
                    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
                    #if CG_DEBUG
                        lout << "Gamepad touch is not supported yet!" << nlaf;
                    #endif
                        break;
                    case SDL_EVENT_FINGER_DOWN:
                    case SDL_EVENT_FINGER_UP:
                    case SDL_EVENT_FINGER_CANCELED:
                    case SDL_EVENT_FINGER_MOTION:
                    #if CG_DEBUG
                        lout << "Touch is not supported yet!" << nlaf;
                    #endif
                        break;
                    case SDL_EVENT_PEN_DOWN:
                    case SDL_EVENT_PEN_UP:
                    case SDL_EVENT_PEN_MOTION:
                    case SDL_EVENT_PEN_AXIS:
                    #if CG_DEBUG
                        lout << "Pen is not supported yet!" << nlaf;
                    #endif
                        break;
                    default:
                    #if CG_DEBUG
                        lout << "Got this event type, fyi: " << event.type << nlaf;
                    #endif
                        break;
                }
            }
        }
    #if CG_DEBUG
        if (i == MAXIMUM_INPUT_EVENTS_PER_FRAME) lout << "[InputHandler] Maximum events per frame reached: " << MAXIMUM_INPUT_EVENTS_PER_FRAME << nlaf;
    #endif
    }

    inline void processPersist() noexcept {
        BoolInput::processPersist();
        MouseMove::processPersist();
        Stick::processPersist();
    }

//#endregion
}