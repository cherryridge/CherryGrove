#pragma once
#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include "../types.hpp"
#include "MMAction.hpp"

namespace InputHandler::MouseMove {
    using glm::vec2;

//#region Actionwise API

    [[nodiscard]] ActionID add(MouseMoveActionCallback cb, ActionPriority priority, const ActionwiseInfo_MM& info) noexcept;
    [[nodiscard]] bool remove(ActionID id) noexcept;

    [[nodiscard]] bool get(ActionHandle handle, MouseMoveAction& result) noexcept;
    [[nodiscard]] bool get(ActionID id        , MouseMoveAction& result) noexcept;

//#endregion

//#region Event Processing

    //threaded: Simulation thread
    void processTrigger(const SDL_Event& event) noexcept;
    void processPersist() noexcept;

//#endregion

//#region Misc

    //Returns
    [[nodiscard]] vec2 getMousePos() noexcept;

//#endregion
}