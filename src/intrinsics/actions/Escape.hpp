#pragma once
#include <atomic>

#include "../../input/Action.hpp"
#include "../../input/boolInput/BIAction.hpp"
#include "../../Main.hpp"
#include "../../simulation/Simulation.hpp"
#include "../../util/SlotTable.hpp"

namespace IntrinsicInput {
    using std::memory_order_release;
    using Util::SlotTable, InputHandler::ActionHandle, InputHandler::EventControlFlags, InputHandler::BoolInput::BoolInputAction, InputHandler::BoolInput::EventwiseInfo_BI;

    inline void escapeCB(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        if (Simulation::gameStarted) Simulation::gameStopSignal.store(true, memory_order_release);
        else Main::isCGAlive.store(false, memory_order_release);
    }
}