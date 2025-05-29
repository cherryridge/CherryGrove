#pragma once

#include "../sources/boolInput.hpp"
#include "../inputBase.hpp"

namespace IntrinsicInput {
    using namespace InputHandler;
    EventFlags escapeCB(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept;
}