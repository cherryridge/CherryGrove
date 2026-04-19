#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"
#include "../../simulation/Time.hpp"
#include "../../util/SlotTable.hpp"
#include "../InputHandler.hpp"
#include "../types.hpp"
#include "../utils.hpp"
#include "biid.hpp"
#include "BindingRecord.hpp"
#include "BIAction.hpp"
#include "KeyCombo.hpp"
#include "KeyState.hpp"

namespace InputHandler::BoolInput {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::lower_bound, std::array, std::atomic, std::vector, Simulation::TimePoint, Simulation::TimeUnit, Util::SlotTable, boost::unordered_flat_map;

    namespace detail {
        inline array<KeyState, BIID_COUNT> state{}, stateSnapshot{};
        inline atomic<u32> snapshotSeq{0};

        inline SlotTable<BoolInputAction, ActionHandle> actionInfos;
        inline SlotTable<BindingRecord, BindingHandle> bindings;
        inline array<vector<BindingHandle>, BIID_COUNT> BIIDtoBindings;

        [[nodiscard]] inline KeyCombo getCurrentCombo() noexcept {
            KeyCombo result;
            for (BoolInputID i = 0; i < BIID_COUNT; i++) if (state[i].isVirtualDown) result.addKey(i);
            return result;
        }

        inline void removeBinding(BindingHandle bindingHandle) noexcept {
            const BindingRecord& record = *bindings.get(bindingHandle);
            for (BoolInputID i = 0; i < BIID_COUNT; i++) if (record.combo.hasKey(i)) {
                auto& vec = BIIDtoBindings[i];
                for (u64 j = 0; j < vec.size(); j++) if (vec[j] == bindingHandle) {
                    vec.erase(vec.begin() + j);
                    break;
                }
            }
            static_cast<void>(bindings.destroy(bindingHandle));
        }
    }

//#region Actionwise API

    //There is no guarantee that any of the action's property isn't duplicated anywhere. Implement it in UMI.
    [[nodiscard]] inline ActionID add(BoolInputActionCallback cb, ActionPriority priority, const ActionwiseInfo_BI& info) noexcept {
        const ActionID id = InputHandler::internal::getNextId();
    #if CG_DEBUG
        if (info.allowedKinds.none()) [[unlikely]] lerr << "[InputHandler::BoolInput] Action " << id << " is not listening to any BoolInputKind. This action will never be triggered." << endl;
    #endif
        const ActionHandle handle = detail::actionInfos.emplace(id, priority, cb, info);
        InputHandler::internal::registerId(id, {InputKind::BoolInput, handle});
        return id;
    }

    //There is no guarantee that global InputHandler deletion is allowed because performance issues of loading an atomic bool every fucking time. Please make sure to enable deletion before calling this function, or very bad things can and will happen.
    //note: This function cannot be overloaded with `ActionHandle` because we need to unregister the action ID in global InputHandler.
    [[nodiscard]] inline bool remove(ActionID id) noexcept {
    #if CG_DEBUG
        ASSERT_CAN_DELETE(id, false)
    #endif
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        if (!detail::actionInfos.destroy(location.actionHandle)) return false;
        InputHandler::internal::unregisterId(id);
        for (auto it = detail::bindings.begin(); it != detail::bindings.end(); ++it) if (it->actionHandle == location.actionHandle) detail::removeBinding(it.handle());
        return true;
    }

    [[nodiscard]] inline bool get(ActionHandle handle, BoolInputAction& result) noexcept {
        const BoolInputAction* actionInfo = detail::actionInfos.get(handle);
        if (actionInfo == nullptr) return false;
        result = *actionInfo;
        return true;
    }

    [[nodiscard]] inline bool get(ActionID id, BoolInputAction& result) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return get(location.actionHandle, result);
    }

//#endregion

//#region Binding API

    [[nodiscard]] inline bool bindingExists(ActionHandle handle, const KeyCombo& combo) noexcept;
    [[nodiscard]] inline bool addBinding(ActionHandle handle, const KeyCombo& combo) noexcept {
        //Check for duplicate bindings. We allow multiple same bindings on different actions, but not on the same action, because it will fire the same callback twice for no reason.
        //Unbound combos are disallowed to be added.
        if (bindingExists(handle, combo) || combo.keyCount() == 0) return false;
    #if CG_DEBUG
        if (combo.keyCount() == 0) {
            lerr << "[InputHandler] Attempt to bind an unbound KeyCombo. This should have been prevented by the caller. ActionHandle: " << handle.value << endl;
            return false;
        }
    #endif
        const BindingHandle bindingHandle = detail::bindings.emplace(handle, combo);
        const auto* actionInfo = detail::actionInfos.get(handle);
        for (BoolInputID i = 0; i < BIID_COUNT; i++) if (combo.hasKey(i)) {
            auto it = lower_bound(detail::BIIDtoBindings[i].begin(), detail::BIIDtoBindings[i].end(), bindingHandle, [actionInfo](BindingHandle existingHandle, BindingHandle) {
                const auto* existingAction = detail::actionInfos.get(detail::bindings.get(existingHandle)->actionHandle);
                if (actionInfo->priority != existingAction->priority) return actionInfo->priority > existingAction->priority;
                else return actionInfo->actionId > existingAction->actionId;
                //note: We don't need to sort for the same action being bound to different combos that share the same key (eg. binding "Shift + A" and "A" for the same action and we're sorting "A"). We DO NOT provide key combo callback invocation consistency.
            });
            detail::BIIDtoBindings[i].insert(it, bindingHandle);
        }
        return true;
    }

    //Will not make `addBindings` with vector input because there is no room for internal optimization.
    [[nodiscard]] inline bool addBinding(ActionID id, const KeyCombo& combo) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return addBinding(location.actionHandle, combo);
    }


    [[nodiscard]] inline bool bindingExists(ActionHandle handle, const KeyCombo& combo) noexcept {
        //Unbound combos are UNBOUND COMBOS. We can either interpret it as "can trigger every action but is not reachable by user" or "cannot trigger any action", and we choose the latter. This means one must remove every binding from an action to make it "unbound".
        if (combo.keyCount() == 0) return false;
        for (BoolInputID i = 0; i < BIID_COUNT; i++) if (combo.hasKey(i)) {
            for (u64 j = 0; j < detail::BIIDtoBindings[i].size(); j++) {
                const BindingRecord* record = detail::bindings.get(detail::BIIDtoBindings[i][j]);
            #if CG_DEBUG
                ASSERT_NOT_NULLPTR(record, false)
            #endif
                if (record->actionHandle == handle && record->combo == combo) return true;
            }
            //If the combo has a key that is not bound to the action, then the binding definitely doesn't exist. No need to check other keys.
            return false;
        }
        return false;
    }

    [[nodiscard]] inline bool bindingExists(ActionID id, const KeyCombo& combo) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return bindingExists(location.actionHandle, combo);
    }


    [[nodiscard]] inline bool getBindings(ActionHandle handle, vector<KeyCombo>& result) noexcept {
        //note: This variable cannot be replaced by `result.empty()` because we want to allow the caller to reuse the `result` vector across multiple calls to avoid unnecessary allocations and they might not clear it between calls.
        bool found = false;
        for (const auto& binding : detail::bindings) if (binding.actionHandle == handle) {
            if (!found) found = true;
            result.push_back(binding.combo);
        }
        return found;
    }

    [[nodiscard]] inline bool getBindings(ActionID id, vector<KeyCombo>& result) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return getBindings(location.actionHandle, result);
    }


    [[nodiscard]] inline bool removeBinding(ActionHandle handle, const KeyCombo& combo) noexcept {
        if (combo.keyCount() == 0) return false;
        for (BoolInputID i = 0; i < BIID_COUNT; i++) if (combo.hasKey(i)) {
            for (u64 j = 0; j < detail::BIIDtoBindings[i].size(); j++) {
                const BindingRecord* record = detail::bindings.get(detail::BIIDtoBindings[i][j]);
            #if CG_DEBUG
                ASSERT_NOT_NULLPTR(record, false)
            #endif
                if (record->actionHandle == handle && record->combo == combo) {
                    detail::removeBinding(detail::BIIDtoBindings[i][j]);
                    return true;
                }
            }
            // If the combo has a key that is not bound to the action, then the binding definitely doesn't exist (so we can't remove). No need to check other keys.
            return false;
        }
        return false;
    }

    [[nodiscard]] inline bool removeBinding(ActionID id, const KeyCombo& combo) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return removeBinding(location.actionHandle, combo);
    }


    [[nodiscard]] inline bool removeBindings(ActionHandle handle) noexcept {
        bool found = false;
        for (auto it = detail::bindings.begin(); it != detail::bindings.end(); ++it) if (it->actionHandle == handle) {
            if (!found) found = true;
            detail::removeBinding(it.handle());
        }
        return found;
    }

    [[nodiscard]] inline bool removeBindings(ActionID id) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return removeBindings(location.actionHandle);
    }


    [[nodiscard]] inline bool getActions(const KeyCombo& combo, vector<BoolInputAction>& result) noexcept {
        if (combo.keyCount() == 0) return false;
        for (BoolInputID i = 0; i < BIID_COUNT; i++) if (combo.hasKey(i)) {
            //note: This variable cannot be replaced by `result.empty()` because we want to allow the caller to reuse the `result` vector across multiple calls to avoid unnecessary allocations and they might not clear it between calls.
            bool found = false;
            for (u64 j = 0; j < detail::BIIDtoBindings[i].size(); j++) {
                const BindingRecord* record = detail::bindings.get(detail::BIIDtoBindings[i][j]);
            #if CG_DEBUG
                ASSERT_NOT_NULLPTR(record, false)
            #endif
                if (record->combo == combo) {
                    const BoolInputAction* action = detail::actionInfos.get(record->actionHandle);
                #if CG_DEBUG
                    ASSERT_NOT_NULLPTR(action, false)
                #endif
                    if (!found) found = true;
                    result.push_back(*action);
                }
            }
            return found;
        }
        return false;
    }

//#endregion

//#region Event Processing

    //threaded: Simulation thread
    inline void processTrigger(const SDL_Event& event) noexcept {
        BoolInputID biid;
        bool down = true;
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                biid = getBIID(BIInputSource::Keyboard, event.key.scancode);
                down = true;
                break;
            case SDL_EVENT_KEY_UP:
                biid = getBIID(BIInputSource::Keyboard, event.key.scancode);
                down = false;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                biid = getBIID(BIInputSource::Mouse, event.button.button);
                down = true;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                biid = getBIID(BIInputSource::Mouse, event.button.button);
                down = false;
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                biid = getBIID(BIInputSource::Gamepad, event.gbutton.button);
                down = true;
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                biid = getBIID(BIInputSource::Gamepad, event.gbutton.button);
                down = false;
                break;
            default:
            #if CG_DEBUG
                lerr << "[InputHandler::BoolInput] Unexpected event type: " << event.type << endl;
            #endif
                return;
        }
        const BoolInputKind triggeredKind = down ? BoolInputKind::Down : BoolInputKind::Up;

        if (down) detail::state[biid].onPhysicalPress(Simulation::now());
        else detail::state[biid].onPhysicalRelease(Simulation::now());

        InputHandler::writeSnapshot(detail::state, detail::stateSnapshot, detail::snapshotSeq);

        KeyCombo currentCombo = detail::getCurrentCombo();
        //note: — Why bother the released one since we have TTL so the released one will definitely be virtually down? — Because players can set TTL to 0. Feel the fucking frustration of coding in ultra flexibility hell!
        //And mind you, we've already removed the released key before calling this function.
        if (!down) currentCombo.addKey(biid);

        unordered_flat_map<KeyCombo, vector<ActionHandle>> comboGroups;
        for (u64 i = 0; i < detail::BIIDtoBindings[biid].size(); i++) {
            const BindingRecord* record = detail::bindings.get(detail::BIIDtoBindings[biid][i]);
        #if CG_DEBUG
            ASSERT_NOT_NULLPTR(record, )
            if (record->combo.keyCount() == 0) {
                lerr << "[InputHandler] BindingRecord with unbound KeyCombo found in BIIDtoBindings. Unbound KeyCombos should not enter the record slottable. BIID: " << biid << ", record index: " << i << endl;
                return;
            }
        #endif
            const BoolInputAction* actionInfo = detail::actionInfos.get(record->actionHandle);
        #if CG_DEBUG
            ASSERT_NOT_NULLPTR(actionInfo, )
        #endif
            //We've sorted the binding handles in the BIIDtoBindings according to the actions behind, so we saved the time of sorting here.
            //Ordered splices of sorted sequences are still sorted, regardless of the order of the splices.
            if (currentCombo >= record->combo && actionInfo->actionwiseInfo.allowedKinds.get(triggeredKind)) comboGroups[record->combo].push_back(record->actionHandle);
        }

        for (const auto& [combo, actions] : comboGroups) InputHandler::process(detail::actionInfos, actions, {
            .triggeredCombo = combo,
            .repeatedTriggerCount = detail::state[biid].tapCount,
            .lastActiveId = biid,
            .triggeredKind = triggeredKind
        });
    }

    //threaded: Simulation thread
    inline void processPersist() noexcept {
        const KeyCombo currentCombo = detail::getCurrentCombo();
        if (currentCombo.keyCount() == 0) return;

        unordered_flat_map<KeyCombo, vector<ActionHandle>> comboGroups2;
        for (BoolInputID biid = 0; biid < BIID_COUNT; biid++) if (currentCombo.hasKey(biid)) {
            for (u64 i = 0; i < detail::BIIDtoBindings[biid].size(); i++) {
                const BindingRecord* record = detail::bindings.get(detail::BIIDtoBindings[biid][i]);
            #if CG_DEBUG
                ASSERT_NOT_NULLPTR(record, )
                if (record->combo.keyCount() == 0) {
                    lerr << "[InputHandler] BindingRecord with unbound KeyCombo found in BIIDtoBindings. Unbound KeyCombos should not enter the record slottable. BIID: " << biid << ", record index: " << i << endl;
                    return;
                }
            #endif
                const BoolInputAction* actionInfo = detail::actionInfos.get(record->actionHandle);
            #if CG_DEBUG
                ASSERT_NOT_NULLPTR(actionInfo, )
            #endif
                //We used an even smarter trick: make every combo only added through the lowest key in the combo. This way we don't need to worry about duplicating actions and every actions in one combo is still sorted, because they are originated from the same BIIDtoBindings list, which is sorted.
                //Also, the first key check comes before because it's more likely to fail and is more performant.
                if (record->combo.firstKey() == biid && currentCombo >= record->combo && actionInfo->actionwiseInfo.allowedKinds.get(BoolInputKind::Persist)) comboGroups2[record->combo].push_back(record->actionHandle);
            }
        }

        for (const auto& [combo, actions] : comboGroups2) InputHandler::process(detail::actionInfos, actions, {
            .triggeredCombo = combo,
            .repeatedTriggerCount = 1,
            .lastActiveId = INVALID_BIID,
            .triggeredKind = BoolInputKind::Persist
        });
    }

    namespace detail {
        inline void resetRange(BoolInputID start, BoolInputID end) noexcept { for (BoolInputID i = start; i <= end; i++) state[i].reset(); }
    }

    //threaded: Simulation thread
    //We don't reset internal states on device addition, only on removal.
    inline void processDevice(const SDL_Event& event) noexcept {
        switch (event.type) {
            case SDL_EVENT_KEYBOARD_REMOVED:
                detail::resetRange(BIID_KEYBOARD_START, BIID_KEYBOARD_END);
                break;
            case SDL_EVENT_MOUSE_REMOVED:
                detail::resetRange(BIID_MOUSE_START, BIID_MOUSE_END);
                break;
            case SDL_EVENT_GAMEPAD_REMOVED:
                detail::resetRange(BIID_GAMEPAD_START, BIID_GAMEPAD_END);
                break;
            default:
            #if CG_DEBUG
                lerr << "[InputHandler] This event type should not be routed to BoolInput::processDevice: " << event.type << endl;
            #endif
                break;
        }
    }

//#region Misc

    //Currently not sure will be called by which thread :)
    inline void updateArguments(TimeUnit repeatTapGap, TimeUnit comboMinTTL) noexcept {
        KeyState::setRepeatTapGap(repeatTapGap);
        KeyState::setComboMinTTL(comboMinTTL);
    }

    //This function DOES NOT guarantee to return the most up-to-date states in very high frequency calls due to hot path prioritization. It only guarantees that the returned states are consistent with each other, meaning that they are from the same snapshot.
    [[nodiscard]] inline array<KeyState, BIID_COUNT> getStates() noexcept {
        return InputHandler::readSnapshot(detail::stateSnapshot, detail::snapshotSeq);
    }

//#endregion
}