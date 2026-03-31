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
#include "../utils.hpp"
#include "biid.hpp"
#include "BindingRecord.hpp"
#include "BIAction.hpp"
#include "KeyCombo.hpp"
#include "KeyState.hpp"

#include "boolInput.hpp"

namespace InputHandler::BoolInput {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::lower_bound, std::array, std::atomic, std::vector, Simulation::TimePoint, Simulation::TimeUnit, Util::SlotTable, boost::unordered_flat_map;

    static array<KeyState, BIID_COUNT> state{}, stateSnapshot{};
    static atomic<u32> snapshotSeq{0};

    static SlotTable<BoolInputAction, ActionHandle> actionInfos;
    static SlotTable<BindingRecord, BindingHandle> bindings;
    static array<vector<BindingHandle>, BIID_COUNT> BIIDtoBindings;

    static KeyCombo getCurrentCombo() noexcept {
        KeyCombo result;
        for (BoolInputID i = 0; i < BIID_COUNT; i++) if (state[i].isVirtualDown) result.addKey(i);
        return result;
    }

    static void _removeBinding(BindingHandle bindingHandle) noexcept {
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

//#region Actionwise API

    [[nodiscard]] ActionID add(BoolInputActionCallback cb, ActionPriority priority, const ActionwiseInfo_BI& info) noexcept {
        const ActionID id = InputHandler::internal::getNextId();
    #if CG_DEBUG
        if (info.allowedKinds.none()) [[unlikely]] lerr << "[InputHandler::BoolInput] Action " << id << " is not listening to any BoolInputKind. This action will never be triggered." << endl;
    #endif
        const ActionHandle handle = actionInfos.emplace(id, priority, cb, info);
        InputHandler::internal::registerId(id, {InputKind::BoolInput, handle});
        return id;
    }

    [[nodiscard]] bool remove(ActionID id) noexcept {
    #if CG_DEBUG
        ASSERT_CAN_DELETE(id, false)
    #endif
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        if (!actionInfos.destroy(location.actionHandle)) return false;
        InputHandler::internal::unregisterId(id);
        for (u32 i = 0; i < bindings.storage.size(); i++) {
            const auto& slot = bindings.storage[i];
            if ((slot.generation & 1) == 0) continue;
            if (slot.data.actionHandle == location.actionHandle) _removeBinding(bindings.getCurrentHandle(i));
        }
        return true;
    }

    [[nodiscard]] bool get(ActionHandle handle, BoolInputAction& result) noexcept {
        const BoolInputAction* actionInfo = actionInfos.get(handle);
        if (actionInfo == nullptr) return false;
        result = *actionInfo;
        return true;
    }

    [[nodiscard]] bool get(ActionID id, BoolInputAction& result) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return get(location.actionHandle, result);
    }

//#endregion

//#region Binding API

    [[nodiscard]] bool addBinding(ActionHandle handle, const KeyCombo& combo) noexcept {
        //Check for duplicate bindings. We allow multiple same bindings on different actions, but not on the same action, because it will fire the same callback twice for no reason.
        //Unbound combos are disallowed to be added.
        if (bindingExists(handle, combo) || combo.keyCount() == 0) return false;
    #if CG_DEBUG
        if (combo.keyCount() == 0) {
            lerr << "[InputHandler] Attempt to bind an unbound KeyCombo. This should have been prevented by the caller. ActionHandle: " << handle.value << endl;
            return false;
        }
    #endif
        const BindingHandle bindingHandle = bindings.emplace(handle, combo);
        const auto* actionInfo = actionInfos.get(handle);
        for (BoolInputID i = 0; i < BIID_COUNT; i++) if (combo.hasKey(i)) {
            auto it = lower_bound(BIIDtoBindings[i].begin(), BIIDtoBindings[i].end(), bindingHandle, [actionInfo](BindingHandle existingHandle, BindingHandle) {
                const auto* existingAction = actionInfos.get(bindings.get(existingHandle)->actionHandle);
                if (actionInfo->priority != existingAction->priority) return actionInfo->priority > existingAction->priority;
                else return actionInfo->actionId > existingAction->actionId;
                //note: We don't need to sort for the same action being bound to different combos that share the same key (eg. binding "Shift + A" and "A" for the same action and we're sorting "A"). We DO NOT provide key combo callback invocation consistency.
            });
            BIIDtoBindings[i].insert(it, bindingHandle);
        }
        return true;
    }

    [[nodiscard]] bool addBinding(ActionID id, const KeyCombo& combo) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return addBinding(location.actionHandle, combo);
    }


    [[nodiscard]] bool bindingExists(ActionHandle handle, const KeyCombo& combo) noexcept {
        //Unbound combos are UNBOUND COMBOS. We can either interpret it as "can trigger every action but is not reachable by user" or "cannot trigger any action", and we choose the latter. This means one must remove every binding from an action to make it "unbound".
        if (combo.keyCount() == 0) return false;
        for (BoolInputID i = 0; i < BIID_COUNT; i++) if (combo.hasKey(i)) {
            for (u64 j = 0; j < BIIDtoBindings[i].size(); j++) {
                const BindingRecord* record = bindings.get(BIIDtoBindings[i][j]);
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

    [[nodiscard]] bool bindingExists(ActionID id, const KeyCombo& combo) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return bindingExists(location.actionHandle, combo);
    }


    [[nodiscard]] bool getBindings(ActionHandle handle, vector<KeyCombo>& result) noexcept {
        //note: This variable cannot be replaced by `result.empty()` because we want to allow the caller to reuse the `result` vector across multiple calls to avoid unnecessary allocations and they might not clear it between calls.
        bool found = false;
        for (u32 i = 0; i < bindings.storage.size(); i++) {
            const auto& slot = bindings.storage[i];
            if ((slot.generation & 1) == 0) continue;
            if (slot.data.actionHandle == handle) {
                if (!found) found = true;
                result.push_back(slot.data.combo);
            }
        }
        return found;
    }

    [[nodiscard]] bool getBindings(ActionID id, vector<KeyCombo>& result) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return getBindings(location.actionHandle, result);
    }


    [[nodiscard]] bool removeBinding(ActionHandle handle, const KeyCombo& combo) noexcept {
        if (combo.keyCount() == 0) return false;
        for (BoolInputID i = 0; i < BIID_COUNT; i++) if (combo.hasKey(i)) {
            for (u64 j = 0; j < BIIDtoBindings[i].size(); j++) {
                const BindingRecord* record = bindings.get(BIIDtoBindings[i][j]);
            #if CG_DEBUG
                ASSERT_NOT_NULLPTR(record, false)
            #endif
                if (record->actionHandle == handle && record->combo == combo) {
                    _removeBinding(BIIDtoBindings[i][j]);
                    return true;
                }
            }
            // If the combo has a key that is not bound to the action, then the binding definitely doesn't exist (so we can't remove). No need to check other keys.
            return false;
        }
        return false;
    }

    [[nodiscard]] bool removeBinding(ActionID id, const KeyCombo& combo) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return removeBinding(location.actionHandle, combo);
    }


    [[nodiscard]] bool removeBindings(ActionHandle handle) noexcept {
        bool found = false;
        for (u32 i = 0; i < bindings.storage.size(); i++) {
            const auto& slot = bindings.storage[i];
            if ((slot.generation & 1) == 0) continue;
            if (slot.data.actionHandle == handle) {
                if (!found) found = true;
                _removeBinding(bindings.getCurrentHandle(i));
            }
        }
        return found;
    }

    [[nodiscard]] bool removeBindings(ActionID id) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::BoolInput)) return false;
        return removeBindings(location.actionHandle);
    }


    [[nodiscard]] bool getActions(const KeyCombo& combo, vector<BoolInputAction>& result) noexcept {
        if (combo.keyCount() == 0) return false;
        for (BoolInputID i = 0; i < BIID_COUNT; i++) if (combo.hasKey(i)) {
            //note: This variable cannot be replaced by `result.empty()` because we want to allow the caller to reuse the `result` vector across multiple calls to avoid unnecessary allocations and they might not clear it between calls.
            bool found = false;
            for (u64 j = 0; j < BIIDtoBindings[i].size(); j++) {
                const BindingRecord* record = bindings.get(BIIDtoBindings[i][j]);
            #if CG_DEBUG
                ASSERT_NOT_NULLPTR(record, false)
            #endif
                if (record->combo == combo) {
                    const BoolInputAction* action = actionInfos.get(record->actionHandle);
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

    void processTrigger(const SDL_Event& event) noexcept {
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
        const InputHandler::BoolInputKind triggeredKind = down ? InputHandler::BoolInputKind::KeyDown : InputHandler::BoolInputKind::KeyUp;

        if (down) state[biid].onPhysicalPress(Simulation::now());
        else state[biid].onPhysicalRelease(Simulation::now());

        InputHandler::writeSnapshot(state, stateSnapshot, snapshotSeq);

        KeyCombo currentCombo = getCurrentCombo();
        //note: — Why bother the released one since we have TTL so the released one will definitely be virtually down? — Because players can set TTL to 0. Feel the fucking frustration of coding in ultra flexibility hell!
        //And mind you, we've already removed the released key before calling this function.
        if (!down) currentCombo.addKey(biid);

        unordered_flat_map<KeyCombo, vector<ActionHandle>> comboGroups;
        for (u64 i = 0; i < BIIDtoBindings[biid].size(); i++) {
            const BindingRecord* record = bindings.get(BIIDtoBindings[biid][i]);
        #if CG_DEBUG
            ASSERT_NOT_NULLPTR(record, )
            if (record->combo.keyCount() == 0) {
                lerr << "[InputHandler] BindingRecord with unbound KeyCombo found in BIIDtoBindings. Unbound KeyCombos should not enter the record slottable. BIID: " << biid << ", record index: " << i << endl;
                return;
            }
        #endif
            const BoolInputAction* actionInfo = actionInfos.get(record->actionHandle);
        #if CG_DEBUG
            ASSERT_NOT_NULLPTR(actionInfo, )
        #endif
            //We've sorted the binding handles in the BIIDtoBindings according to the actions behind, so we saved the time of sorting here.
            //Ordered splices of sorted sequences are still sorted, regardless of the order of the splices.
            if (currentCombo >= record->combo && actionInfo->actionwiseInfo.allowedKinds.get(triggeredKind)) comboGroups[record->combo].push_back(record->actionHandle);
        }

        for (const auto& [combo, actions] : comboGroups) InputHandler::process(actionInfos, actions, {
            .triggeredCombo = combo,
            .repeatedTriggerCount = state[biid].tapCount,
            .lastActiveId = biid,
            .triggeredKind = triggeredKind
        });
    }

    void processPersist() noexcept {
        const KeyCombo currentCombo = getCurrentCombo();
        if (currentCombo.keyCount() == 0) return;

        unordered_flat_map<KeyCombo, vector<ActionHandle>> comboGroups2;
        for (BoolInputID biid = 0; biid < BIID_COUNT; biid++) if (currentCombo.hasKey(biid)) {
            for (u64 i = 0; i < BIIDtoBindings[biid].size(); i++) {
                const BindingRecord* record = bindings.get(BIIDtoBindings[biid][i]);
            #if CG_DEBUG
                ASSERT_NOT_NULLPTR(record, )
                if (record->combo.keyCount() == 0) {
                    lerr << "[InputHandler] BindingRecord with unbound KeyCombo found in BIIDtoBindings. Unbound KeyCombos should not enter the record slottable. BIID: " << biid << ", record index: " << i << endl;
                    return;
                }
            #endif
                const BoolInputAction* actionInfo = actionInfos.get(record->actionHandle);
            #if CG_DEBUG
                ASSERT_NOT_NULLPTR(actionInfo, )
            #endif
                //We used an even smarter trick: make every combo only added through the lowest key in the combo. This way we don't need to worry about duplicating actions and every actions in one combo is still sorted, because they are originated from the same BIIDtoBindings list, which is sorted.
                //Also, the first key check comes before because it's more likely to fail and is more performant.
                if (record->combo.firstKey() == biid && currentCombo >= record->combo && actionInfo->actionwiseInfo.allowedKinds.get(InputHandler::BoolInputKind::KeyPersist)) comboGroups2[record->combo].push_back(record->actionHandle);
            }
        }

        for (const auto& [combo, actions] : comboGroups2) InputHandler::process(actionInfos, actions, {
            .triggeredCombo = combo,
            .repeatedTriggerCount = 1,
            .lastActiveId = INVALID_BIID,
            .triggeredKind = InputHandler::BoolInputKind::KeyPersist
        });
    }

    static void _resetRange(BoolInputID start, BoolInputID end) noexcept { for (BoolInputID i = start; i <= end; i++) state[i].reset(); }

    //We don't reset internal states on device addition, only on removal.
    void processDevice(const SDL_Event& event) noexcept {
        switch (event.type) {
            case SDL_EVENT_KEYBOARD_REMOVED:
                _resetRange(BIID_KEYBOARD_START, BIID_KEYBOARD_END);
                break;
            case SDL_EVENT_MOUSE_REMOVED:
                _resetRange(BIID_MOUSE_START, BIID_MOUSE_END);
                break;
            case SDL_EVENT_GAMEPAD_REMOVED:
                _resetRange(BIID_GAMEPAD_START, BIID_GAMEPAD_END);
                break;
            default:
            #if CG_DEBUG
                lerr << "[InputHandler] This event type should not be routed to BoolInput::processDevice: " << event.type << endl;
            #endif
                break;
        }
    }

    //Currently not sure will be called by which thread :)
    void updateArguments(TimeUnit repeatTapGap, TimeUnit comboMinTTL) noexcept {
        KeyState::setRepeatTapGap(repeatTapGap);
        KeyState::setComboMinTTL(comboMinTTL);
    }

    [[nodiscard]] array<KeyState, BIID_COUNT> getStates() noexcept {
        return InputHandler::readSnapshot(stateSnapshot, snapshotSeq);
    }
}