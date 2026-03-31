#include <algorithm>
#include <array>
#include <atomic>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"
#include "../../util/SlotTable.hpp"
#include "../InputHandler.hpp"
#include "../utils.hpp"
#include "STAction.hpp"

#include "stick.hpp"

namespace InputHandler::Stick {
    typedef uint8_t u8;
    typedef int16_t i16;
    typedef uint32_t u32;
    using std::atomic, std::sqrt, std::clamp, std::array, std::underlying_type_t, std::numeric_limits, std::pair, std::vector, Util::SlotTable;

    //[LeftStickK, LeftStickB, RightStickK, RightStickB, LeftTriggerK, LeftTriggerB, RightTriggerK, RightTriggerB]
    static array<float, 8> deadzones{};
    //note: We must store the raw values in `state` and apply deadzones before the callbacks to allow `processPersist` to work with raw values later.
    static array<i16, static_cast<underlying_type_t<Axis>>(Axis::Count)> state{}, stateSnapshot{};
    static atomic<u32> snapshotSeq{0};

    static SlotTable<StickAction, ActionHandle> actionInfos;
    static array<vector<ActionHandle>, static_cast<underlying_type_t<Axis>>(Axis::Count)> axisToHandle;

    [[nodiscard]] ActionID add(StickActionCallback cb, ActionPriority priority, const ActionwiseInfo_ST& info) noexcept {
        const ActionID id = InputHandler::internal::getNextId();
    #if CG_DEBUG
        if (info.triggerAxises.none()) [[unlikely]] lerr << "[InputHandler] Attempt to add a StickAction with no axis. This action will never be triggered. ActionID: " << id << endl;
    #endif
        const ActionHandle handle = actionInfos.emplace(id, priority, cb, info);
        for (u8 axis = 0; axis < static_cast<underlying_type_t<Axis>>(Axis::Count); axis++) if (info.triggerAxises.get(static_cast<Axis>(axis))) {
            auto it = axisToHandle[axis].begin();
            for (; it != axisToHandle[axis].end(); ++it) if (
                priority > actionInfos.get(*it)->priority
             || (
                    priority == actionInfos.get(*it)->priority
                 && id > actionInfos.get(*it)->actionId
                )
            ) break;
            axisToHandle[axis].insert(it, handle);
        }
        InputHandler::internal::registerId(id, {InputKind::StickMove, handle});
        return id;
    }

    [[nodiscard]] bool remove(ActionID id) noexcept {
    #if CG_DEBUG
        ASSERT_CAN_DELETE(id, false)
    #endif
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::StickMove)) return false;
        const auto* actionInfo = actionInfos.get(location.actionHandle);
        if (actionInfo == nullptr) return false;
        const auto triggerAxises = actionInfo->actionwiseInfo.triggerAxises;
        static_cast<void>(actionInfos.destroy(location.actionHandle));
        for (u8 axis = 0; axis < static_cast<underlying_type_t<Axis>>(Axis::Count); axis++) if (triggerAxises.get(static_cast<Axis>(axis))) for (u64 i = 0; i < axisToHandle[axis].size(); i++) if (axisToHandle[axis][i] == location.actionHandle) {
            axisToHandle[axis].erase(axisToHandle[axis].begin() + i);
            break;
        }
        InputHandler::internal::unregisterId(id);
        return true;
    }

    [[nodiscard]] bool get(ActionHandle handle, StickAction& result) noexcept {
        const StickAction* actionInfo = actionInfos.get(handle);
        if (actionInfo == nullptr) return false;
        result = *actionInfo;
        return true;
    }

    [[nodiscard]] bool get(ActionID id, StickAction& result) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::StickMove)) return false;
        return get(location.actionHandle, result);
    }

    static pair<float, float> _regulate2D(u8 axisIndex, u8 deadzoneIndex) noexcept {
        const float x = static_cast<float>(state[axisIndex]) / numeric_limits<i16>::max(), y = static_cast<float>(state[axisIndex + 1]) / numeric_limits<i16>::max();
        const float magnitude = sqrt(x * x + y * y);
        if (magnitude == 0.0f) return {0.0f, 0.0f};
        const float normalizedMagnitude = clamp(magnitude * deadzones[deadzoneIndex] + deadzones[deadzoneIndex + 1], 0.0f, 1.0f);
        return {x * normalizedMagnitude / magnitude, y * normalizedMagnitude / magnitude};
    }

    static float _regulate1D(u8 axisIndex, u8 deadzoneIndex) noexcept {
        const float value = static_cast<float>(state[axisIndex]) / numeric_limits<i16>::max();
        return clamp(value * deadzones[deadzoneIndex] + deadzones[deadzoneIndex + 1], 0.0f, 1.0f);
    }

    void processTrigger(const SDL_Event& event) noexcept {
    #if CG_DEBUG
        if (event.type != SDL_EVENT_GAMEPAD_AXIS_MOTION) [[unlikely]] {
            lerr << "[InputHandler::Stick] Unexpected event type: " << event.type << endl;
            return;
        }
    #endif
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
                //The whole `axisIndex` is JUST FOR THIS CASE SO WE PEOPLE DON'T GET CONFUSED. IN ANY OTHER CASES IT'S JUST THE SAME AS `axis`.
                axisIndex = 5;
                deadzoneIndex = 6;
                break;
        }

        state[static_cast<underlying_type_t<Axis>>(axis)] = event.gaxis.value;
        InputHandler::writeSnapshot(state, stateSnapshot, snapshotSeq);

        EventwiseInfo_ST eventwiseInfo{
            .axis = axis,
            .triggeredKind = InputHandler::Stick::Subkind::Trigger
        };

        if (axis == Axis::LeftX || axis == Axis::LeftY || axis == Axis::RightX || axis == Axis::RightY) {
            const auto [x, y] = _regulate2D(axisIndex, deadzoneIndex);
            eventwiseInfo.values[0] = x;
            eventwiseInfo.values[1] = y;
        }
        else eventwiseInfo.values[0] = _regulate1D(axisIndex, deadzoneIndex);

        vector<ActionHandle> triggerKindHandles;
        for (u64 i = 0; i < axisToHandle[static_cast<underlying_type_t<Axis>>(axis)].size(); i++) {
            const auto* actionInfo = actionInfos.get(axisToHandle[static_cast<underlying_type_t<Axis>>(axis)][i]);
        #if CG_DEBUG
            ASSERT_NOT_NULLPTR(actionInfo, )
        #endif
            if (actionInfo->actionwiseInfo.allowedKinds.get(InputHandler::Stick::Subkind::Trigger)) triggerKindHandles.push_back(axisToHandle[static_cast<underlying_type_t<Axis>>(axis)][i]);
        }

        InputHandler::process(actionInfos, triggerKindHandles, eventwiseInfo);
    }

    void processPersist() noexcept {
        vector<ActionHandle> persistKindHandles;
        for (u8 axis = 0; axis < static_cast<underlying_type_t<Axis>>(Axis::Count); axis++) for (u64 i = 0; i < axisToHandle[axis].size(); i++) {
            const auto* actionInfo = actionInfos.get(axisToHandle[axis][i]);
        #if CG_DEBUG
            ASSERT_NOT_NULLPTR(actionInfo, )
        #endif
            if (actionInfo->actionwiseInfo.allowedKinds.get(InputHandler::Stick::Subkind::Persist)) persistKindHandles.push_back(axisToHandle[axis][i]);
        }

        const auto [leftStickX, leftStickY] = _regulate2D(0, 0);
        const auto [rightStickX, rightStickY] = _regulate2D(2, 2);
        const auto leftTrigger = _regulate1D(4, 4), rightTrigger = _regulate1D(5, 6);

        const EventwiseInfo_ST eventwiseInfo{
            .values = {leftStickX, leftStickY, rightStickX, rightStickY, leftTrigger, rightTrigger},
            .axis = Axis::Count,
            .triggeredKind = InputHandler::Stick::Subkind::Persist
        };
        InputHandler::process(actionInfos, persistKindHandles, eventwiseInfo);
    }

    void processDevice(const SDL_Event& event) noexcept {
    #if CG_DEBUG
        if (event.type != SDL_EVENT_GAMEPAD_REMOVED) [[unlikely]] {
            lerr << "[InputHandler::Stick] Unexpected event type: " << event.type << endl;
            return;
        }
    #endif
        state.fill(0);
        InputHandler::writeSnapshot(state, stateSnapshot, snapshotSeq);
    }

    void updateArguments(array<i16, 8> deadzones_) noexcept {
        for (u8 i = 0; i < 8; i++) deadzones_[i] = clamp(deadzones_[i], static_cast<i16>(0), numeric_limits<i16>::max());
        for (u8 i = 0; i < 8; i += 2) {
            const float range = static_cast<float>(deadzones_[i + 1]) - static_cast<float>(deadzones_[i]);
            if (range > 0.0f) {
                deadzones[i] = static_cast<float>(numeric_limits<i16>::max()) / range;
                deadzones[i + 1] = -static_cast<float>(deadzones_[i]) / range;
            }
            else {
                deadzones[i] = 0.0f;
                deadzones[i + 1] = 0.0f;
            }
        }
    }

    [[nodiscard]] array<i16, static_cast<underlying_type_t<Axis>>(Axis::Count)> getStates() noexcept { return InputHandler::readSnapshot(stateSnapshot, snapshotSeq); }
}