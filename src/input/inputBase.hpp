#pragma once
#include <atomic>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace InputHandler {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef u8 EventFlags;
    typedef u32 EventPriority;
    typedef u32 ActionID;
    using std::string, std::atomic, std::function, std::vector, std::move, std::shared_ptr, std::make_shared, std::numeric_limits, std::memory_order_acq_rel, std::memory_order_relaxed, std::memory_order_acquire;

    struct MousePos {
        float x, y;
    };

    //Eventflags constants
    inline constexpr u8 EVENTFLAGS_CANCEL_PROMPTED = 1;
    inline constexpr u8 EVENTFLAGS_STOP_AFTER = 1 << 1;
    inline constexpr u8 EVENTFLAGS_STOP_IMMEDIATELY = 1 << 2;
    //end Eventflags constants

    struct ActionInfo {
        string nameAndSpace;
        ActionID eventId;
        EventPriority priority;
    };

    template<typename ActionType, typename EventDataType>
    using CallbackTemplate = function<EventFlags(const vector<ActionType>& events, const ActionInfo& info, const EventDataType& data, EventFlags flags)>;

    //Thank ChatGPT
    template<typename ActionType>
    struct ActionRegistryTemplate {
        //I want to express `unatomic`!
        ActionRegistryTemplate() noexcept { actions.store(make_shared<const vector<ActionType>>(), memory_order_relaxed); }

        //Drop the pointer after use or you will cause a memory leak.
        auto getPtr() const noexcept { return actions.load(memory_order_acquire); }

        //We are sticking with CAS model for now because we *might* need to write from more than one thread.
        //Yes, I know there is something called `runOnMainThread`, but the appropriate blocking mechanism is not yet built and is generally only for debug use.
        void operateSwap(function<bool(vector<ActionType>& original)> operation) noexcept {
            while (true) {
                auto oldPtr = actions.load(memory_order_acquire);
                vector<ActionType> oldVec = *oldPtr;
                auto performSwap = operation(oldVec);
                if (!performSwap) break;
                auto newPtr = make_shared<const vector<ActionType>>(move(oldVec));
                if (actions.compare_exchange_weak(oldPtr, newPtr, memory_order_acq_rel, memory_order_acquire)) break;
            }
        }

        //Returns the index of the thing. If not present, returns `numeric_limits<u32>::max()`.
        u32 has(ActionID id) const noexcept {
            //Done it in higher abstraction.
            //if (id == INVALID_ACTION_ID) return false;
            //This is for strong holding the vector to stop it from being destructed.
            auto snapshotPtr = getPtr();
            const auto& vec = *snapshotPtr;
            for (u32 i = 0; i < vec.size(); i++) if (vec[i].info.eventId == id) return i;
            return numeric_limits<u32>::max();
        }

    private:
        atomic<shared_ptr<const vector<ActionType>>> actions;
    };

    inline constexpr ActionID INVALID_ACTION_ID = 0;
    inline atomic<ActionID> nextId(1);

    inline ActionID getNextId() noexcept { return nextId.fetch_add(1, std::memory_order_relaxed); }
} // namespace InputHandler