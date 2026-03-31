#pragma once
#include <cstdint>

namespace InputHandler {
    typedef uint8_t u8;

    enum struct InputKind : u8 {
        BoolInput,
        MouseMove,
        Scroll,
        StickMove,
        GamepadTouch,
        Touch,
        Pen,
        Count
    };

    //note: Reason for why there is no `KeyCancelled`:
    //Generally SDL3 will do its best to synthesize `SDL_EVENT_KEY_UP` when the window/app loses focus.
    //HOWEVER, Gemini said that on some customized Android systems, SDL3 might not get enough time to emit any event before the system backgrounds the app. If the system allows parts of the app to run in background that happens to be related to the input system, we're in big trouble. So, make sure to test that later if we're supporting mobile.
    enum struct BoolInputKind : u8 {
        KeyDown,
        KeyPersist,
        KeyUp,
        Count
    };

    enum struct TouchKind : u8 {
        TouchDown,
        TouchPersist,
        TouchUp,
        TouchCancelled,
        Count
    };
}