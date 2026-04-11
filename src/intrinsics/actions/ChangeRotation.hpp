#pragma once
#include <atomic>

#include "../../input/mouseMove/MMAction.hpp"
#include "../../input/Action.hpp"
#include "../../simulation/Simulation.hpp"
#include "../../systems/Rotation.hpp"

namespace IntrinsicInput {
    using std::atomic, Util::SlotTable, InputHandler::ActionHandle, InputHandler::EventControlFlags, InputHandler::MouseMove::MouseMoveAction, InputHandler::MouseMove::EventwiseInfo_MM;

    inline atomic<double> cameraSensitivity{0.05};

    inline void changeRotationCB(const SlotTable<MouseMoveAction, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo_MM& eventwiseInfo, EventControlFlags& flags) noexcept {
        double dX = eventwiseInfo.deltaX * cameraSensitivity, dY = eventwiseInfo.deltaY * cameraSensitivity;
        if (dX == 0) dX = Components::Rotation::infinity;
        else if (dY == 0) dY = Components::Rotation::infinity;
        Components::Rotation::deltaRotation(Simulation::playerEntity, dX, dY);
    }
}