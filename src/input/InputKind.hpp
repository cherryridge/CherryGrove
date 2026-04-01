#pragma once
#include <cstdint>

namespace InputHandler {
    typedef uint8_t u8;

    enum struct InputKind : u8 {
        BoolInput, MouseMove, Scroll, StickMove, GamepadTouch, Touch, Pen,
        Count
    };

    //todo: Move this shit to `touch` when we're writing touch code.
    //enum struct TouchKind : u8 {
    //    TouchDown,
    //    TouchPersist,
    //    TouchUp,
    //    TouchCancelled,
    //    Count
    //};
}