#pragma once
#include <cstdint>

#include "../../util/BitField.hpp"
#include "../Action.hpp"
#include "biid.hpp"
#include "KeyCombo.hpp"

namespace InputHandler::BoolInput {
    typedef uint8_t u8;
    typedef uint64_t u64;
    using Util::BitField;

    //note: Reason for why there is no `KeyCancelled`:
    //Generally SDL3 will do its best to synthesize `SDL_EVENT_KEY_UP` when the window/app loses focus.
    //HOWEVER, Gemini said that on some customized Android systems, SDL3 might not get enough time to emit any event before the system backgrounds the app. If the system allows parts of the app to run in background that happens to be related to the input system, we're in big trouble. So, make sure to test that later if we're supporting mobile.
    enum struct BoolInputKind : u8 {
        Down, Persist, Up,
        Count
    };

    struct ActionwiseInfo_BI {
        BitField<BoolInputKind, BoolInputKind::Count> allowedKinds;
    };

    struct EventwiseInfo_BI {
        KeyCombo triggeredCombo;
        //This is only meant for Down/Up (triggered) events in combos.
        //For Persist events, this field is always 1 to remain consistent with triggered events.
        u64 repeatedTriggerCount;
        //This is only meant for Down/Up (triggered) events in combos to allow callbacks to identify the last key user pressed/released.
        //When the combo is an one key combo, this field is just the triggered key.
        //For Persist, this field is INVALID_BIID.
        BoolInputID lastActiveId;
        BoolInputKind triggeredKind;
    };

    using BoolInputAction = Action<ActionwiseInfo_BI, EventwiseInfo_BI>;
    using BoolInputActionCallback = ActionCallback<ActionwiseInfo_BI, EventwiseInfo_BI>;
}