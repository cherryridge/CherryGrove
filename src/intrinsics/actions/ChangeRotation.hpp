#pragma once

#include "../../input/Action.hpp"
#include "../../input/mouseMove/MMAction.hpp"
#include "../../settings/Settings.hpp"
#include "../../simulation/playerEntity.hpp"
#include "../systems/Rotation.hpp"

namespace IntrinsicInput {
    using Util::SlotTable, InputHandler::ActionHandle, InputHandler::EventControlFlags, InputHandler::MouseMove::MouseMoveAction, InputHandler::MouseMove::EventwiseInfo_MM;

    inline void changeRotationCB(const SlotTable<MouseMoveAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_MM& eventwiseInfo, EventControlFlags& flags) noexcept {
        const float
            cameraSensitivity = Settings::getSettings().input.mouseSensitivity * 0.15f,
            dX = eventwiseInfo.deltaX * cameraSensitivity,
            dY = eventwiseInfo.deltaY * cameraSensitivity;
        static_cast<void>(Systems::deltaRotation(Simulation::playerEntity, dX, dY));
        flags |= InputHandler::EVENT_CONTROL_FLAGS_STOP_PROMPTED;
    }
}