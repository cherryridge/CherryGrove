#pragma once
#include <atomic>

#include "../sources/mouseMove.hpp"
#include "../inputBase.hpp"

namespace IntrinsicInput {
    using namespace InputHandler;
    extern std::atomic<double> cameraSensitivity;

    EventFlags changeRotationCB(const vector<MouseMove::Action>& events, const ActionInfo& info, const MouseMove::EventData& data, EventFlags flags) noexcept;
}