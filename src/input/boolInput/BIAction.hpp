#pragma once

#include "../../util/BitField.hpp"
#include "../Action.hpp"
#include "../InputKind.hpp"
#include "biid.hpp"
#include "KeyCombo.hpp"

namespace InputHandler::BoolInput {
    typedef uint64_t u64;
    using InputHandler::BoolInputKind, InputHandler::BoolInputKind, Util::BitField;

    struct ActionwiseInfo_BI {
        BitField<BoolInputKind, BoolInputKind::Count> allowedKinds;
    };

    struct EventwiseInfo_BI {
        KeyCombo triggeredCombo;
        //This is only meant for KeyDown/KeyUp (triggered) events in combos.
        //For KeyPersist events, this field is always 1 to remain consistent with triggered events.
        u64 repeatedTriggerCount;
        //This is only meant for KeyDown/KeyUp (triggered) events in combos to allow callbacks to identify the last key user pressed/released.
        //When the combo is an one key combo, this field is just the triggered key.
        //For KeyPersist, this field is INVALID_BIID.
        BoolInputID lastActiveId;
        BoolInputKind triggeredKind;
    };

    using BoolInputAction = Action<ActionwiseInfo_BI, EventwiseInfo_BI>;
    using BoolInputActionCallback = ActionCallback<ActionwiseInfo_BI, EventwiseInfo_BI>;
}