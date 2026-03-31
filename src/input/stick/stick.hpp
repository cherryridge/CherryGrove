#pragma once
#include <array>
#include <type_traits>
#include <SDL3/SDL.h>

#include "../types.hpp"
#include "STAction.hpp"

namespace InputHandler::Stick {
    typedef int16_t i16;
    typedef uint16_t u16;
    using std::array, std::underlying_type_t;

//#region Actionwise API

    [[nodiscard]] ActionID add(StickActionCallback cb, ActionPriority priority, const ActionwiseInfo_ST& info) noexcept;
    [[nodiscard]] bool remove(ActionID id) noexcept;

    [[nodiscard]] bool get(ActionHandle handle, StickAction& result) noexcept;
    [[nodiscard]] bool get(ActionID id        , StickAction& result) noexcept;

//#endregion

//#region Event Processing

    //threaded: Simulation thread
    void processTrigger(const SDL_Event& event) noexcept;
    void processPersist() noexcept;
    void processDevice(const SDL_Event& event) noexcept;

//#endregion

//#region Misc

    //[LeftStickMin, LeftStickMax, RightStickMin, RightStickMax, LeftTriggerMin, LeftTriggerMax, RightTriggerMin, RightTriggerMax]
    void updateArguments(array<i16, 8> deadzones_) noexcept;

    //This function DOES NOT guarantee to return the most up-to-date states in very high frequency calls due to hot path prioritization. It only guarantees that the returned states are consistent with each other, meaning that they are from the same snapshot.
    [[nodiscard]] array<i16, static_cast<underlying_type_t<Axis>>(Axis::Count)> getStates() noexcept;

//#endregion
}