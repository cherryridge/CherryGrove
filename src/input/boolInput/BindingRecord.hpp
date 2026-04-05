#pragma once
#include <cstdint>

#include "../../util/SlotTable.hpp"
#include "../Action.hpp"
#include "KeyCombo.hpp"

namespace InputHandler::BoolInput {
    typedef uint64_t u64;
    using Util::GenerationalHandle;

    struct BindingRecord {
        ActionHandle actionHandle;
        KeyCombo combo;
    };

    MAKE_DISTINCT_HANDLE(BindingHandle)
}