#pragma once
#include <array>
#include <type_traits>

#include "../../util/BitField.hpp"
#include "../Action.hpp"

namespace InputHandler::Stick {
    typedef uint8_t u8;
    typedef int16_t i16;
    using std::array, std::underlying_type_t, Util::BitField;

    enum struct Subkind : u8 {
        Trigger, Persist,
        Count
    };

    enum struct Axis : u8 {
        LeftX, LeftY, RightX, RightY, LeftTrigger, RightTrigger,
        Count
    };

    struct ActionwiseInfo_ST {
        BitField<Subkind, Subkind::Count> allowedKinds;
        BitField<Axis, Axis::Count> triggerAxises;
    };

    struct EventwiseInfo_ST {
        //If `triggeredKind` is `Trigger`:
        //  If `axis` is `Axis::(Left/Right)(X/Y)`, values[0] is regulated X value and values[1] is regulated Y value.
        //  If `axis` is `Axis::(Left/Right)Trigger`, values[0] is the regulated trigger value.
        //  Other values are always 0.
        //Else if `triggeredKind` is `Persist`, this field is every regulated value of all axises.
        array<float, static_cast<underlying_type_t<Axis>>(Axis::Count)> values;
        //If `triggeredKind` is `Persist`, this field is `Axis::Count`.
        Axis axis;
        Subkind triggeredKind;
    };

    using StickAction = Action<ActionwiseInfo_ST, EventwiseInfo_ST>;
    using StickActionCallback = ActionCallback<ActionwiseInfo_ST, EventwiseInfo_ST>;
}