#pragma once
#include <atomic>

#include "../../input/Action.hpp"
#include "../../input/boolInput/BIAction.hpp"
#include "../../globalState.hpp"
#include "../../simulation/states.hpp"
#include "../../util/SlotTable.hpp"

namespace IntrinsicInput {
    using std::memory_order_acquire, Util::SlotTable, InputHandler::ActionHandle, InputHandler::EventControlFlags, InputHandler::BoolInput::BoolInputAction, InputHandler::BoolInput::EventwiseInfo_BI;

    inline void escapeCB(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        if (Simulation::isSimStarted()) Simulation::signalExit();
        else GlobalState::setIsCGAlive(false);
    }
}