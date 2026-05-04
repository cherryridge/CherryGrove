#pragma once
#include <cstdint>

#include "../../util/BitField.hpp"
#include "../Action.hpp"

namespace InputHandler::Scroll {
    typedef uint8_t u8;
    typedef int32_t i32;
    using Util::BitField;

    enum struct Direction : u8 {
        Horizontal, Vertical,
        Count
    };

    struct ActionwiseInfo_SC {
        //Bit 0 for horizontal, bit 1 for vertical.
        //The other direction's true value will still be passed to the callback!
        BitField<Direction, Direction::Count> directions;
    };

    struct EventwiseInfo_SC {
        float scrollX, scrollY;
        //Additional information for the mouse position.
        float mouseX, mouseY;
        i32 discreteX, discreteY;
        //true if SDL_EVENT_MOUSE_WHEEL_FLIPPED is set.
        //note: The values above are ALREADY processed according to the direction so you don't need to multiply by -1 again. This is just fyi.
        bool inverted;
    };

    using ScrollAction = Action<ActionwiseInfo_SC, EventwiseInfo_SC>;
    using ScrollActionCallback = ActionCallback<ActionwiseInfo_SC, EventwiseInfo_SC>;
}