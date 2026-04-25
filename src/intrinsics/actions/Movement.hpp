#pragma once

#include "../../input/Action.hpp"
#include "../../input/boolInput/BIAction.hpp"
#include "../../simulation/playerEntity.hpp"
#include "../../util/SlotTable.hpp"
#include "../systems/fly.hpp"
#include "../systems/Rotation.hpp"

namespace IntrinsicInput {
    using Util::SlotTable, InputHandler::ActionHandle, InputHandler::EventControlFlags, InputHandler::BoolInput::BoolInputAction, InputHandler::BoolInput::EventwiseInfo_BI;

    inline void forward(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        const auto* rotation = Simulation::registry.try_get<Components::Rotation>(Simulation::playerEntity);
        if (rotation) static_cast<void>(Systems::fly(Simulation::playerEntity, Systems::Directions::forward(*rotation)));
    }

    inline void backward(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        const auto* rotation = Simulation::registry.try_get<Components::Rotation>(Simulation::playerEntity);
        if (rotation) static_cast<void>(Systems::fly(Simulation::playerEntity, Systems::Directions::backwards(*rotation)));
    }

    inline void left(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        const auto* rotation = Simulation::registry.try_get<Components::Rotation>(Simulation::playerEntity);
        if (rotation) static_cast<void>(Systems::fly(Simulation::playerEntity, Systems::Directions::left(*rotation)));
    }

    inline void right(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        const auto* rotation = Simulation::registry.try_get<Components::Rotation>(Simulation::playerEntity);
        if (rotation) static_cast<void>(Systems::fly(Simulation::playerEntity, Systems::Directions::right(*rotation)));
    }

    inline void up(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        const auto* rotation = Simulation::registry.try_get<Components::Rotation>(Simulation::playerEntity);
        if (rotation) static_cast<void>(Systems::fly(Simulation::playerEntity, Systems::Directions::up(*rotation)));
    }

    inline void down(const SlotTable<BoolInputAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_BI& eventwiseInfo, EventControlFlags& flags) noexcept {
        const auto* rotation = Simulation::registry.try_get<Components::Rotation>(Simulation::playerEntity);
        if (rotation) static_cast<void>(Systems::fly(Simulation::playerEntity, Systems::Directions::down(*rotation)));
    }
}