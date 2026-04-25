#pragma once
#include <SDL3/SDL.h>

#include "../types.hpp"
#include "SCAction.hpp"

namespace InputHandler::Scroll {

//#region Actionwise API

    [[nodiscard]] ActionID add(ScrollActionCallback cb, ActionPriority priority, const ActionwiseInfo_SC& info) noexcept;
    [[nodiscard]] bool remove(ActionID id) noexcept;

    [[nodiscard]] bool get(ActionHandle handle, ScrollAction& result) noexcept;
    [[nodiscard]] bool get(ActionID id        , ScrollAction& result) noexcept;

//#endregion

//#region Event Processing

    //threaded: Simulation thread
    void processTrigger(const SDL_Event& event) noexcept;

//#endregion
}