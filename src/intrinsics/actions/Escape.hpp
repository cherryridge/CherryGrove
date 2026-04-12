#pragma once
#include <atomic>

#include "../../input/Action.hpp"
#include "../../input/boolInput/BIAction.hpp"
#include "../../globalState.hpp"
#include "../../main/hold.hpp"
#include "../../simulation/Simulation.hpp"
#include "../../util/SlotTable.hpp"

namespace IntrinsicInput {
    using std::memory_order_acquire, std::memory_order_release, Util::SlotTable, InputHandler::ActionHandle, InputHandler::EventControlFlags, InputHandler::BoolInput::BoolInputAction, InputHandler::BoolInput::EventwiseInfo_BI;

    inline void escapeCB(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        if (Simulation::gameStarted.load(memory_order_acquire)) Main::runOnMainThread.enqueue(Simulation::exit);
        else GlobalState::isCGAlive.store(false, memory_order_release);
    }
}