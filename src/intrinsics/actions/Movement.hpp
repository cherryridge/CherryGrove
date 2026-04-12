#pragma once

#include "../../input/Action.hpp"
#include "../../input/boolInput/BIAction.hpp"
#include "../../settings/Settings.hpp"
#include "../../simulation/playerEntity.hpp"
#include "../../systems/fly.hpp"
#include "../../util/SlotTable.hpp"

namespace IntrinsicInput {
    using Util::SlotTable, InputHandler::ActionHandle, InputHandler::EventControlFlags, InputHandler::BoolInput::BoolInputAction, InputHandler::BoolInput::EventwiseInfo_BI;

    inline void forward(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        Components::Coordinates::forwards(Simulation::playerEntity, moveSensitivity);
    }

    inline void backward(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        Components::Coordinates::backwards(Simulation::playerEntity, moveSensitivity);
    }

    inline void left(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        Components::Coordinates::strafeLeft(Simulation::playerEntity, moveSensitivity);
    }

    inline void right(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        Components::Coordinates::strafeRight(Simulation::playerEntity, moveSensitivity);
    }

    inline void up(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        Components::Coordinates::fly(Simulation::playerEntity, flySensitivity);
    }

    inline void down(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        Components::Coordinates::fly(Simulation::playerEntity, -flySensitivity);
    }
}