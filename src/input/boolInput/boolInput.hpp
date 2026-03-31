#pragma once
#include <array>
#include <vector>
#include <SDL3/SDL.h>

#include "../../simulation/Time.hpp"
#include "../types.hpp"
#include "BIAction.hpp"
#include "KeyCombo.hpp"
#include "KeyState.hpp"

namespace InputHandler::BoolInput {
    using std::array, std::vector, Simulation::TimeUnit;

//#region Actionwise API

    //There is no guarantee that any of the action's property isn't duplicated anywhere. Implement it in UMI.
    [[nodiscard]] ActionID add(BoolInputActionCallback cb, ActionPriority priority, const ActionwiseInfo_BI& info) noexcept;

    //There is no guarantee that global InputHandler deletion is allowed because performance issues of loading an atomic bool every fucking time. Please make sure to enable deletion before calling this function, or very bad things can and will happen.
    //note: This function cannot be overloaded with `ActionHandle` because we need to unregister the action ID in global InputHandler.
    [[nodiscard]] bool remove(ActionID id) noexcept;

    [[nodiscard]] bool get(ActionHandle handle, BoolInputAction& result) noexcept;
    [[nodiscard]] bool get(ActionID id        , BoolInputAction& result) noexcept;

//#endregion

//#region Binding API

    //Will not make `addBindings` with vector input because there is no room for internal optimization.
    [[nodiscard]] bool addBinding(ActionHandle handle, const KeyCombo& combo) noexcept;
    [[nodiscard]] bool addBinding(ActionID id        , const KeyCombo& combo) noexcept;

    [[nodiscard]] bool bindingExists(ActionHandle handle, const KeyCombo& combo) noexcept;
    [[nodiscard]] bool bindingExists(ActionID id        , const KeyCombo& combo) noexcept;

    [[nodiscard]] bool getBindings(ActionHandle handle, vector<KeyCombo>& result) noexcept;
    [[nodiscard]] bool getBindings(ActionID id        , vector<KeyCombo>& result) noexcept;

    [[nodiscard]] bool removeBinding(ActionHandle handle, const KeyCombo& combo) noexcept;
    [[nodiscard]] bool removeBinding(ActionID id        , const KeyCombo& combo) noexcept;

    [[nodiscard]] bool removeBindings(ActionHandle handle) noexcept;
    [[nodiscard]] bool removeBindings(ActionID id        ) noexcept;

    [[nodiscard]] bool getActions(const KeyCombo& combo, vector<BoolInputAction>& result) noexcept;

//#endregion

//#region Event Processing

    //threaded: Simulation thread
    void processTrigger(const SDL_Event& event) noexcept;
    void processPersist() noexcept;
    void processDevice(const SDL_Event& event) noexcept;

//#endregion

//#region Misc

    void updateArguments(TimeUnit repeatTapGap, TimeUnit comboMinTTL) noexcept;

    //This function DOES NOT guarantee to return the most up-to-date states in very high frequency calls due to hot path prioritization. It only guarantees that the returned states are consistent with each other, meaning that they are from the same snapshot.
    [[nodiscard]] array<KeyState, BIID_COUNT> getStates() noexcept;

//#endregion
}