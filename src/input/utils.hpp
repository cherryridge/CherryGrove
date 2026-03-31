#pragma once
#include <atomic>
#include <vector>

#include "../util/SlotTable.hpp"
#include "Action.hpp"
#include "InputHandler.hpp"
#include "types.hpp"

namespace InputHandler {
    typedef uint8_t u8;
    typedef uint32_t u32;
    using std::atomic, std::atomic_thread_fence, std::memory_order_acquire, std::memory_order_relaxed, std::memory_order_release, std::vector, Util::SlotTable;

    //SeqLock-based snapshot: single writer, multiple readers.
    //Writer side: copy source into dest with seq protection.
    template <typename T>
    inline void writeSnapshot(const T& source, T& dest, atomic<u32>& seq) noexcept {
        const u32 s = seq.load(memory_order_relaxed);
        seq.store(s + 1, memory_order_release); //Odd → write in progress.
        dest = source;
        seq.store(s + 2, memory_order_release); //Even → write complete.
        seq.notify_all();
    }

    //Reader side: spin-read until a consistent (untorn) snapshot is obtained.
    template <typename T>
    [[nodiscard]] inline T readSnapshot(const T& snapshot, atomic<u32>& seq) noexcept {
        T snapshotCopy;
        u32 currentSeq;
        do {
            currentSeq = seq.load(memory_order_acquire);
            if (currentSeq & 1) {
                //Writer is in progress; wait for it to finish.
                seq.wait(currentSeq, memory_order_relaxed);
                continue;
            }
            snapshotCopy = snapshot;
            atomic_thread_fence(memory_order_acquire); //Ensure data reads complete before the seq re-check.
        } while (seq.load(memory_order_relaxed) != currentSeq);
        return snapshotCopy;
    }

    template <typename ActionwiseInfo, typename EventwiseInfo>
    inline void process(const SlotTable<Action<ActionwiseInfo, EventwiseInfo>, ActionHandle>& actionInfos, const vector<ActionHandle>& actions, const EventwiseInfo& eventwiseInfo) noexcept {
        ActionPriority stopPriority = 0;
        u8 currentFlags = 0;
        for (u64 i = 0; i < actions.size(); i++) {
            const auto* action = actionInfos.get(actions[i]);
        #if CG_DEBUG
            ASSERT_NOT_NULLPTR(action, )
        #endif
            if (currentFlags & EVENT_CONTROL_FLAGS_STOP_AT_LOWER_PRIORITY && stopPriority != action->priority) break;
            action->callback(actionInfos, actions[i], eventwiseInfo, currentFlags);
            if (currentFlags & EVENT_CONTROL_FLAGS_STOP_IMMEDIATELY) break;
            if (currentFlags & EVENT_CONTROL_FLAGS_STOP_AT_LOWER_PRIORITY) stopPriority = action->priority;
        }
    }

    template <typename ActionwiseInfo, typename EventwiseInfo>
    inline void insertSort(vector<ActionHandle>& vec, const SlotTable<Action<ActionwiseInfo, EventwiseInfo>, ActionHandle>& actionInfos, ActionHandle newHandle) {
        const auto* newAction = actionInfos.get(newHandle);
    #if CG_DEBUG
        ASSERT_NOT_NULLPTR(newAction, )
    #endif
        auto it = vec.begin();
        for (; it != vec.end(); ++it) {
            const auto* action = actionInfos.get(*it);
        #if CG_DEBUG
            ASSERT_NOT_NULLPTR(action, )
        #endif
            if (
                newAction->priority > action->priority
             || (
                    newAction->priority == action->priority
                 && newAction->actionId > action->actionId
                )
            ) break;
        }
        vec.insert(it, newHandle);
    }

    template <typename ActionwiseInfo, typename EventwiseInfo>
    inline void remove(vector<ActionHandle>& vec, const SlotTable<Action<ActionwiseInfo, EventwiseInfo>, ActionHandle>& actionInfos, ActionID id) {
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            const auto* action = actionInfos.get(*it);
        #if CG_DEBUG
            ASSERT_NOT_NULLPTR(action, )
        #endif
            if (action->actionId == id) {
                vec.erase(it);
                break;
            }
        }
    }
}