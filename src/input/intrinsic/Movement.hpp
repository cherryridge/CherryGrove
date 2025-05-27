#pragma once
#include <atomic>

#include "../inputBase.hpp"
#include "../sources/boolInput.hpp"

namespace IntrinsicInput {
    using namespace InputHandler;

    extern std::atomic<double> moveSensitivity;
    extern std::atomic<double> flySensitivity;

    void forward(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId);
    void backward(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId);
    void left(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId);
    void right(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId);

    void up(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId);
    void down(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId);
}