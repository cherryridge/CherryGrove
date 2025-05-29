#pragma once
#include <atomic>

#include "../sources/boolInput.hpp"
#include "../inputBase.hpp"

namespace IntrinsicInput {
    using std::atomic;
    using namespace InputHandler;

    extern atomic<double> moveSensitivity, flySensitivity;

    EventFlags forward(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept;
    EventFlags backward(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept;
    EventFlags left(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept;
    EventFlags right(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept;

    EventFlags up(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept;
    EventFlags down(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept;
}