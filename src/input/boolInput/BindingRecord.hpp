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

    struct BindingHandle {
        GenerationalHandle value;

        bool operator==(const BindingHandle& other) const noexcept { return value == other.value; }
    };
}