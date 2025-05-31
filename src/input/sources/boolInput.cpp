#include <algorithm>
#include <array>
#include <bitset>
#include <limits>
#include <shared_mutex>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"
#include "../InputHandler.hpp"
#include "../inputBase.hpp"
#include "boolInput.hpp"

namespace InputHandler::BoolInput {
    typedef uint8_t u8;
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::array, std::shared_mutex, std::shared_lock, std::scoped_lock, std::move, std::numeric_limits, std::bitset, std::sort;

    //todo: Lock-free biStates
    static bitset<BID_COUNT> biStates, gamepadReset;
    static shared_mutex biStatesMutex;

    static array<ActionRegistryTemplate<Action>, BID_COUNT>
        biInactiveRegistries,
        biPressRegistries,
        biRepeatRegistries,
        biReleaseRegistries;
    #define GET_REGISTRIES(type) type == ActionTypes::Inactive ? biInactiveRegistries : type == ActionTypes::Press ? biPressRegistries : type == ActionTypes::Repeat ? biRepeatRegistries : biReleaseRegistries

    void init() noexcept {
        //Construct gamepad reset mask bitset.
        gamepadReset.set();
        for (BoolInputID i = BID_GAMEPAD_START; i <= BID_GAMEPAD_END; i++) gamepadReset.reset(i);
    }

    const bitset<BID_COUNT>& getKeyStates() noexcept { return biStates; }

    //Doesn't lock the state array!
    inline static void setState(BoolInputID bID, bool state) noexcept {
        //scoped_lock lock(biStatesMutex);
        biStates.set(bID, state);
    }

    ActionID addBoolInput(const string& nameAndSpace, EventPriority priority, CallbackTemplate<Action, EventData> cb, ActionTypes type, BoolInputID defaultBinding) noexcept {
        if (defaultBinding < BID_COUNT) {
            auto& registry = (GET_REGISTRIES(type))[defaultBinding];
            auto id = getNextId();
            registry.operateSwap([&nameAndSpace, &priority, &cb, &defaultBinding, &id](vector<Action>& original) {
                original.emplace_back(ActionInfo{nameAndSpace, id, priority}, cb, defaultBinding);
                sort(original.begin(), original.end(), [](const Action& a, const Action& b) {
                    return a.info.priority < b.info.priority;
                });
                return true;
            });
            return id;
        }
        else {
            lerr << "[InputHandler] Passed invalid bool input ID for default binding: " << defaultBinding << endl;
            return INVALID_ACTION_ID;
        }
    }

    bool removeBoolInput(ActionID id, ActionTypes type) noexcept {
        if (id == INVALID_ACTION_ID) return false;
        auto& registries = GET_REGISTRIES(type);
        //Should check `EMPTY_BID`.
        for (BoolInputID i = 0; i < BID_COUNT; i++) {
            bool succeed = false;
            registries[i].operateSwap([&id, &succeed](vector<Action>& original) {
                for (auto it = original.begin(); it != original.end(); it++) if(it->info.eventId == id) {
                    original.erase(it);
                    succeed = true;
                    return true;
                }
                //No matches, cancel the swap
                return false;
            });
            if (succeed) return true;
        }
        return false;
    }

    bool getBinding(ActionID id, ActionTypes type, BoolInputID& result) noexcept {
        if (id == INVALID_ACTION_ID) {
            lerr << "[InputHandler] Passed invalid action id to get binding!" << endl;
            return false;
        }
        const auto& registries = GET_REGISTRIES(type);
        for (BoolInputID i = 0; i < BID_COUNT; i++) if (registries[i].has(id) != numeric_limits<u32>::max()) {
            result = i;
            return true;
        }
        return false;
    }

    bool changeBinding(ActionID id, ActionTypes type, BoolInputID newBID) noexcept {
        if (id == INVALID_ACTION_ID) return false;
        if (newBID < BID_COUNT) {
            auto& registries = GET_REGISTRIES(type);
            bool succeed = false;
            Action temp(true);
            //Should check `EMPTY_BID`.
            for (BoolInputID i = 0; i < BID_COUNT; i++) {
                //Literally `removeBoolInput` code.
                registries[i].operateSwap([&id, &succeed, &temp](vector<Action>& original) {
                    for (auto it = original.begin(); it != original.end(); it++) if (it->info.eventId == id) {
                        temp = *it;
                        original.erase(it);
                        succeed = true;
                        return true;
                    }
                    //No matches, cancel the swap
                    return false;
                });
                if (succeed) {
                    registries[newBID].operateSwap([&temp](vector<Action>& original) {
                        original.push_back(move(temp));
                        sort(original.begin(), original.end(), [](const Action& a, const Action& b) {
                            return a.info.priority < b.info.priority;
                        });
                        return true;
                    });
                    break;
                }
            }
            return succeed;
        }
        else {
            lerr << "[InputHandler] Passed invalid bool input ID: " << newBID << endl;
            return false;
        }
    }

    bool resetBinding(ActionID id, ActionTypes type) noexcept {
        if (id == INVALID_ACTION_ID) return false;
        auto& registries = GET_REGISTRIES(type);
        bool succeed = false;
        Action temp(true);
        //this == changeBinding(..., defaultBinding)
        //Should check `EMPTY_BID`.
        for (BoolInputID i = 0; i < BID_COUNT; i++) {
            registries[i].operateSwap([&id, &succeed, &temp](vector<Action>& original) {
                for (auto it = original.begin(); it != original.end(); it++) if (it->info.eventId == id) {
                    temp = *it;
                    original.erase(it);
                    succeed = true;
                    return true;
                }
                //No matches, cancel the swap
                return false;
            });
            if (succeed) {
                registries[temp.defaultBinding].operateSwap([&temp](vector<Action>& original) {
                    original.push_back(move(temp));
                    sort(original.begin(), original.end(), [](const Action& a, const Action& b) {
                        return a.info.priority < b.info.priority;
                    });
                    return true;
                });
                break;
            }
        }
        return succeed;
    }

    const ActionRegistryTemplate<Action>* getRegistry(ActionTypes type, BoolInputID bID) noexcept {
        if (bID < BID_COUNT) {
            const auto& registries = GET_REGISTRIES(type);
            return &registries[bID];
        }
        else return nullptr;
    }

    static inline void s_process(const vector<Action>& vec, EventData data) noexcept {
        u32 stopPriority = 0;
        u8 currentFlags = 0;
        for (u32 i = 0; i < vec.size(); i++) {
            if (currentFlags & EVENTFLAGS_STOP_AFTER && stopPriority != vec[i].info.priority) break;
            EventFlags flags = vec[i].cb(vec, vec[i].info, data, currentFlags);
            if (flags & EVENTFLAGS_STOP_IMMEDIATELY) break;
            if (flags & EVENTFLAGS_STOP_AFTER) stopPriority = vec[i].info.priority;
            //fixme: Now it's designed that we can't get rid of any flags once they're set. Is it the correct design?
            currentFlags |= flags;
        }
    }

    void processTrigger(const SDL_Event& event, bool updateOnly) noexcept {
        i32 rawCode = 0;
        bool isPress = true;
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                rawCode = event.key.scancode;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                rawCode = 0 - event.button.button;
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                rawCode = 1000 + event.gbutton.button;
                break;
            case SDL_EVENT_KEY_UP:
                rawCode = event.key.scancode;
                isPress = false;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                rawCode = 0 - event.button.button;
                isPress = false;
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                rawCode = 1000 + event.gbutton.button;
                isPress = false;
                break;
            default:
                lerr << "[InputHandler] BoolInput::processTrigger got unexpected event type: " << event.type << endl;
                return;
        }
        auto p = boolInputMap.find(rawCode);
        //Invalid SDL scancode, do nothing
        if (p == boolInputMap.end()) return;
        {
            scoped_lock stateLock(biStatesMutex);
            setState(p->second, isPress);
        }
        if (!updateOnly) {
            auto& registries = isPress ? biPressRegistries : biReleaseRegistries;
            auto snapshotPtr = registries[p->second].getPtr();
            s_process(*snapshotPtr, {p->second});
        }
    }

    void processPersist() noexcept {
        shared_lock lockFetch(biStatesMutex);
        auto _biStates = biStates;
        lockFetch.unlock();
        for (BoolInputID i = 0; i < BID_COUNT; i++) {
            EventData data(i);
            if (_biStates[i]) {
                //Hold it. Don't die
                auto ptrRepeat = biRepeatRegistries[i].getPtr();
                s_process(*ptrRepeat, data);
            }
            else {
                auto ptrInactive = biInactiveRegistries[i].getPtr();
                s_process(*ptrInactive, data);
            }
        }
    }

    void update() noexcept {
        //Use swap technique to shorten lock time.
        shared_lock lockFetch(biStatesMutex);
        auto _biStates = biStates;
        lockFetch.unlock();
        SDL_MouseButtonFlags mouseState = SDL_GetMouseState(nullptr, nullptr);
        for (BoolInputID i = BID_MOUSE_BUTTON_START; i <= BID_MOUSE_BUTTON_END; i++) setState(i, mouseState & (1ull << (-1 - boolInputMapR[i])));
        //-------------------------------------------------
        i32 keyCount = 0;
        const bool* keyState = SDL_GetKeyboardState(&keyCount);
        for (BoolInputID i = BID_KEY_START; i <= BID_KEY_END; i++) {
            if (boolInputMapR[i] < keyCount) setState(i, keyState[boolInputMapR[i]]);
            else lerr << "[InputHandler] Keyboard state out of bound: " << boolInputMapR[i] << " for length " << keyCount << endl;
        }
        //-------------------------------------------------
        if (InputHandler::gamepadHandle != nullptr) for (BoolInputID i = BID_GAMEPAD_START; i <= BID_GAMEPAD_END; i++) setState(i, SDL_GetGamepadButton(InputHandler::gamepadHandle, static_cast<SDL_GamepadButton>(boolInputMapR[i] - 1000)));
        else if (InputHandler::gamepadStateResetSignal) {
            biStates &= gamepadReset;
            gamepadStateResetSignal = false;
        }
        scoped_lock lockCommit(biStatesMutex);
        biStates = _biStates;
    }
} // namespace InputHandler::BoolInput