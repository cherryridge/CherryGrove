#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"
#include "../inputBase.hpp"
#include "scroll.hpp"

namespace InputHandler::Scroll {
    typedef uint8_t u8;
    typedef uint32_t u32;
    using std::shared_mutex, std::sort, std::vector, std::scoped_lock;

    static EventData store;
    static shared_mutex storeMutex;

    static ActionRegistryTemplate<Action> registry;

    ActionID addScroll(const string& nameAndSpace, EventPriority priority, CallbackTemplate<Action, EventData> cb) noexcept {
        auto id = getNextId();
        registry.operateSwap([&nameAndSpace, &priority, &cb, &id](vector<Action>& original) {
            original.emplace_back(ActionInfo{nameAndSpace, id, priority}, cb);
            sort(original.begin(), original.end(), [](const Action& a, const Action& b) {
                return a.info.priority < b.info.priority;
            });
            return true;
        });
        return id;
    }

    bool removeScroll(ActionID id) noexcept {
        if (id == INVALID_ACTION_ID) return false;
        bool succeed = false;
        registry.operateSwap([&id, &succeed](vector<Action>& original) {
            for (auto it = original.begin(); it != original.end(); it++) if (it->info.eventId == id) {
                original.erase(it);
                succeed = true;
                return true;
            }
            //No matches, cancel the swap
            return false;
        });
        return succeed;
    }

    static inline void s_process(const vector<Action>& vec) noexcept {
        u32 stopPriority = 0;
        u8 currentFlags = 0;
        for (u32 i = 0; i < vec.size(); i++) {
            if (currentFlags & EVENTFLAGS_STOP_AFTER && stopPriority != vec[i].info.priority) break;
            EventFlags flags = vec[i].cb(vec, vec[i].info, store, currentFlags);
            if (flags & EVENTFLAGS_STOP_IMMEDIATELY) break;
            if (flags & EVENTFLAGS_STOP_AFTER) stopPriority = vec[i].info.priority;
            //fixme: Now it's designed that we can't get rid of any flags once they're set. Is it the correct design?
            currentFlags |= flags;
        }
    }

    void process(const SDL_Event& event, bool updateOnly) noexcept {
        switch (event.type) {
            case SDL_EVENT_MOUSE_WHEEL: {
                scoped_lock lock(storeMutex);
                store.newX = event.wheel.mouse_x;
                store.newY = event.wheel.mouse_y;
                store.scrollX = event.wheel.x;
                store.scrollY = event.wheel.y;
                break;
            }
            default:
                lerr << "[InputHandler] Scroll::process got unexpected event type: " << event.type << endl;
                return;
        }
        if (!updateOnly) {
            auto snapshotPtr = registry.getPtr();
            s_process(*snapshotPtr);
        }
    }
}