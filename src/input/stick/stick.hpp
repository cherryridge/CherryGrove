#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <limits>
#include <utility>
#include <vector>
#include <SDL3/SDL.h>

#include "../../debug/loggers.hpp"
#include "../../util/SlotTable.hpp"
#include "../actionIds.hpp"
#include "../canDelete.hpp"
#include "../types.hpp"
#include "../utils.hpp"
#include "STAction.hpp"

namespace InputHandler::Stick {
    typedef uint8_t u8;
    typedef int16_t i16;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::atomic, std::sqrt, std::clamp, std::array, std::to_underlying, std::numeric_limits, std::pair, std::vector, Util::SlotTable, InputHandler::internal::ActionLocation, InputHandler::internal::getLocation, InputHandler::internal::getNextId, InputHandler::internal::registerId, InputHandler::internal::unregisterId, InputHandler::utils::readSnapshot, InputHandler::utils::writeSnapshot, InputHandler::utils::process, InputHandler::utils::insertSort;

//#region Actionwise API

    namespace detail {
        //note: We must store the raw values in `state` and apply deadzones before the callbacks to allow `processPersist` to work with raw values later.
        inline array<i16, to_underlying(Axis::Count)> state{}, stateSnapshot{};
        inline atomic<u32> snapshotSeq{0};

        //[LeftStickK, LeftStickB, RightStickK, RightStickB, LeftTriggerK, LeftTriggerB, RightTriggerK, RightTriggerB]
        inline array<float, 8> deadzones{};

        inline SlotTable<StickAction, ActionHandle> actionInfos;
        inline array<vector<ActionHandle>, to_underlying(Axis::Count)> axisToHandle;
        inline vector<ActionHandle> persistHandles;
    }

    [[nodiscard]] inline ActionID add(StickActionCallback cb, ActionPriority priority, const ActionwiseInfo_ST& info) noexcept {
        const ActionID id = getNextId();
        if (info.triggerAxises.none()) [[unlikely]] lerr << "[InputHandler] Attempt to add a StickAction with no axis. This action will never be triggered. ActionID: " << id << nlaf;
        const ActionHandle handle = detail::actionInfos.emplace(id, priority, cb, info);
        for (u8 axis = 0; axis < to_underlying(Axis::Count); axis++) if (info.triggerAxises.get(static_cast<Axis>(axis))) insertSort(detail::axisToHandle[axis], detail::actionInfos, handle);
        if (info.allowedKinds.get(Subkind::Persist)) insertSort(detail::persistHandles, detail::actionInfos, handle);
        registerId(id, {InputKind::StickMove, handle});
        return id;
    }

    [[nodiscard]] inline bool remove(ActionID id) noexcept {
        ASSERT_CAN_DELETE(id, false)
        ActionLocation location;
        if (!getLocation(id, location, InputKind::StickMove)) return false;

        const auto* actionInfo = detail::actionInfos.get(location.actionHandle);
        ASSERT_NOT_NULLPTR(actionInfo, return false;)

        for (u8 axis = 0; axis < to_underlying(Axis::Count); axis++) {
            if (actionInfo->actionwiseInfo.triggerAxises.get(static_cast<Axis>(axis))) {
                for (u64 i = 0; i < detail::axisToHandle[axis].size(); i++) {
                    if (detail::axisToHandle[axis][i] == location.actionHandle) {
                        detail::axisToHandle[axis].erase(detail::axisToHandle[axis].begin() + i);
                        break;
                    }
                }
            }
        }

        if (actionInfo->actionwiseInfo.allowedKinds.get(Subkind::Persist)) {
            for (u64 i = 0; i < detail::persistHandles.size(); i++) {
                if (detail::persistHandles[i] == location.actionHandle) {
                    detail::persistHandles.erase(detail::persistHandles.begin() + i);
                    break;
                }
            }
        }

        static_cast<void>(detail::actionInfos.destroy(location.actionHandle));

        unregisterId(id);
        return true;
    }

    [[nodiscard]] inline bool get(ActionHandle handle, StickAction& result) noexcept {
        const StickAction* actionInfo = detail::actionInfos.get(handle);
        ASSERT_NOT_NULLPTR(actionInfo, return false;)
        result = *actionInfo;
        return true;
    }

    [[nodiscard]] inline bool get(ActionID id, StickAction& result) noexcept {
        ActionLocation location;
        if (!getLocation(id, location, InputKind::StickMove)) return false;
        return get(location.actionHandle, result);
    }

//#endregion

//#region Event Processing

    namespace detail {
        inline pair<float, float> regulate2D(u8 axisIndex, u8 deadzoneIndex) noexcept {
            const float x = static_cast<float>(state[axisIndex]) / numeric_limits<i16>::max(), y = static_cast<float>(state[axisIndex + 1]) / numeric_limits<i16>::max();
            const float magnitude = sqrt(x * x + y * y);
            if (magnitude == 0.0f) return {0.0f, 0.0f};
            const float normalizedMagnitude = clamp(magnitude * deadzones[deadzoneIndex] + deadzones[deadzoneIndex + 1], 0.0f, 1.0f);
            return {x * normalizedMagnitude / magnitude, y * normalizedMagnitude / magnitude};
        }

        inline float regulate1D(u8 axisIndex, u8 deadzoneIndex) noexcept {
            const float value = static_cast<float>(state[axisIndex]) / numeric_limits<i16>::max();
            return clamp(value * deadzones[deadzoneIndex] + deadzones[deadzoneIndex + 1], 0.0f, 1.0f);
        }
    }

    inline void processTrigger(const SDL_Event& event) noexcept {
        if (event.type != SDL_EVENT_GAMEPAD_AXIS_MOTION) [[unlikely]] {
            lerr << "[InputHandler::Stick] Unexpected event type: " << event.type << nlaf;
            return;
        }
        // For sticks, axisIndex is the X-component base index of the pair (passed to _regulate2D, which reads axisIndex and axisIndex+1).
        // For triggers, axisIndex is the state array index of the trigger itself (== to_underlying(axis)).
        Axis axis;
        u8 axisIndex, deadzoneIndex;
        switch (event.gaxis.axis) {
            case SDL_GAMEPAD_AXIS_LEFTX:
                axis = Axis::LeftX;
                axisIndex = 0;
                deadzoneIndex = 0;
                break;
            case SDL_GAMEPAD_AXIS_LEFTY:
                axis = Axis::LeftY;
                axisIndex = 0;
                deadzoneIndex = 0;
                break;
            case SDL_GAMEPAD_AXIS_RIGHTX:
                axis = Axis::RightX;
                axisIndex = 2;
                deadzoneIndex = 2;
                break;
            case SDL_GAMEPAD_AXIS_RIGHTY:
                axis = Axis::RightY;
                axisIndex = 2;
                deadzoneIndex = 2;
                break;
            case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
                axis = Axis::LeftTrigger;
                axisIndex = 4;
                deadzoneIndex = 4;
                break;
            case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
                axis = Axis::RightTrigger;
                axisIndex = 5;
                deadzoneIndex = 6;
                break;
            default: return;
        }

        detail::state[to_underlying(axis)] = event.gaxis.value;
        writeSnapshot(detail::state, detail::stateSnapshot, detail::snapshotSeq);

        EventwiseInfo_ST eventwiseInfo {
            .axis = axis,
            .triggeredKind = InputHandler::Stick::Subkind::Trigger
        };

        if (axis == Axis::LeftX || axis == Axis::LeftY || axis == Axis::RightX || axis == Axis::RightY) {
            const auto [x, y] = detail::regulate2D(axisIndex, deadzoneIndex);
            eventwiseInfo.values[0] = x;
            eventwiseInfo.values[1] = y;
        }
        else eventwiseInfo.values[0] = detail::regulate1D(axisIndex, deadzoneIndex);

        vector<ActionHandle> triggerKindHandles;
        for (u64 i = 0; i < detail::axisToHandle[to_underlying(axis)].size(); i++) {
            const auto* actionInfo = detail::actionInfos.get(detail::axisToHandle[to_underlying(axis)][i]);
            ASSERT_NOT_NULLPTR(actionInfo, continue;)
            if (actionInfo->actionwiseInfo.allowedKinds.get(InputHandler::Stick::Subkind::Trigger)) triggerKindHandles.push_back(detail::axisToHandle[to_underlying(axis)][i]);
        }

        process(detail::actionInfos, triggerKindHandles, eventwiseInfo);
    }

    inline void processPersist() noexcept {
        const auto [leftStickX, leftStickY] = detail::regulate2D(0, 0);
        const auto [rightStickX, rightStickY] = detail::regulate2D(2, 2);
        const auto leftTrigger = detail::regulate1D(4, 4), rightTrigger = detail::regulate1D(5, 6);

        const EventwiseInfo_ST eventwiseInfo{
            .values = {leftStickX, leftStickY, rightStickX, rightStickY, leftTrigger, rightTrigger},
            .axis = Axis::Count,
            .triggeredKind = InputHandler::Stick::Subkind::Persist
        };
        process(detail::actionInfos, detail::persistHandles, eventwiseInfo);
    }

    inline void processDevice(const SDL_Event& event) noexcept {
        if (event.type != SDL_EVENT_GAMEPAD_REMOVED) [[unlikely]] {
            lerr << "[InputHandler::Stick] Unexpected event type: " << event.type << nlaf;
            return;
        }
        detail::state.fill(0);
        writeSnapshot(detail::state, detail::stateSnapshot, detail::snapshotSeq);
    }

//#endregion

//#region Misc

    //[LeftStickMin, LeftStickMax, RightStickMin, RightStickMax, LeftTriggerMin, LeftTriggerMax, RightTriggerMin, RightTriggerMax]
    inline void updateArguments(array<i16, 8> deadzones_) noexcept {
        for (u8 i = 0; i < 8; i++) deadzones_[i] = clamp(deadzones_[i], static_cast<i16>(0), numeric_limits<i16>::max());
        for (u8 i = 0; i < 8; i += 2) {
            const float range = static_cast<float>(deadzones_[i + 1]) - static_cast<float>(deadzones_[i]);
            if (range > 0.0f) {
                detail::deadzones[i] = static_cast<float>(numeric_limits<i16>::max()) / range;
                detail::deadzones[i + 1] = -static_cast<float>(deadzones_[i]) / range;
            }
            else {
                detail::deadzones[i] = 0.0f;
                detail::deadzones[i + 1] = 0.0f;
            }
        }
    }

    //This function DOES NOT guarantee to return the most up-to-date states in very high frequency calls due to hot path prioritization. It only guarantees that the returned states are consistent with each other, meaning that they are from the same snapshot.
    [[nodiscard]] inline array<i16, to_underlying(Axis::Count)> getStates() noexcept { return readSnapshot(detail::stateSnapshot, detail::snapshotSeq); }

//#endregion
}