#pragma once
#include <cstdint>

#include "../../util/BitField.hpp"
#include "../Action.hpp"

namespace InputHandler::MouseMove {
    typedef uint8_t u8;
    using Util::BitField;

    enum struct SubKind : u8 {
        Trigger, Persist,
        Count
    };

    struct ActionwiseInfo_MM {
        BitField<SubKind, SubKind::Count> allowedKinds;
    };

    struct EventwiseInfo_MM {
        float currentX, currentY, deltaX, deltaY;
        //Persist actions will be fired with `deltaX` and `deltaY` equal to `0.0f`.
        SubKind triggeredKind;
    };

    using MouseMoveAction = Action<ActionwiseInfo_MM, EventwiseInfo_MM>;
    using MouseMoveActionCallback = ActionCallback<ActionwiseInfo_MM, EventwiseInfo_MM>;
}