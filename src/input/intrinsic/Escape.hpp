#pragma once
#include "../InputHandler.hpp"

namespace IntrinsicInput {
    using namespace InputHandler;
    void escapeCB(const std::multimap<EventPriority, BoolInput::BIEvent>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId);
}