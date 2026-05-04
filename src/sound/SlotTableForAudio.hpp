#pragma once
#include <atomic>
#include <memory>
#include <vector>
#include <utility>

#include "../util/concepts.hpp"
#include "../util/SlotTable.hpp"

namespace Sound {
    typedef uint8_t u8;
    typedef uint32_t u32;
    using std::memory_order_relaxed, std::memory_order_acquire, std::memory_order_release, std::atomic_ref, std::shared_ptr, std::pair, std::vector, std::move, std::forward, std::make_shared, Util::DistinctHandleOf, Util::GenerationalHandle;

    //It's concurrent read-safe because it's only meant to be modified by the Audio Thread.
    //And it's probably ruined by strong handle types, good luck understanding it.
    template <typename HandleType, typename EntryType> requires DistinctHandleOf<HandleType, GenerationalHandle>
    struct SlotTableForAudio {
        struct Entry {
            shared_ptr<EntryType> data;
            //Generation 0 is for INVALID handles, which are used when adding new entries to the table that is deemed invalid for some reason. Do not change this for God's sake.
            mutable u32 generation{1};
        };

        vector<Entry> storage;
        vector<u32> freeList;

        [[nodiscard]] SlotTableForAudio() noexcept = default;
        [[nodiscard]] SlotTableForAudio(u8 originSizeMagnitude) noexcept { storage.reserve(1ull << originSizeMagnitude); }

        template <typename... Args>
        [[nodiscard]] pair<HandleType, shared_ptr<EntryType>> emplace(Args&&... args) noexcept {
            u32 index = 0;
            if (freeList.empty()) {
                index = static_cast<u32>(storage.size());
                //Create an empty object: generation 1, data holding nullptr.
                storage.emplace_back();
            }
            else {
                index = freeList.back();
                freeList.pop_back();
            }
            Entry& slot = storage[index];
            u32 generation = atomic_ref(slot.generation).load(memory_order_relaxed);
            slot.data = make_shared<EntryType>(forward<Args>(args)...);
            return {
                HandleType{
                    .value = GenerationalHandle{generation, index}
                },
                slot.data
            };
        }

        [[nodiscard]] shared_ptr<EntryType> getPtr(HandleType handle) const noexcept {
            const GenerationalHandle inner = handle.value;
            auto generation = inner.getGeneration(), index = inner.getIndex();
            if (index >= storage.size()) return {};
            const auto& slot = storage[index];
            if (atomic_ref(slot.generation).load(memory_order_acquire) != generation) return {};
            return slot.data;
        }

        //It might not actually destroy the shared pointer if other places still hold it.
        //WARNING: NO OTHER CODE SHOULD EVER HOLD ANY SHARED POINTER IN THE SLOTTABLE PERSISTENTLY, OR YOU WILL LEAK THE MEMORY!
        [[nodiscard]] bool destroy(HandleType handle) noexcept {
            const GenerationalHandle inner = handle.value;
            const auto index = inner.getIndex();
            if (index >= storage.size()) return false;
            auto& slot = storage[index];
            //Only the Audio Thread is allowed to modify the data, so we can compare then add without worrying about `generation` being modified by someone else on the middle.
            if (atomic_ref(slot.generation).load(memory_order_acquire) != inner.getGeneration()) return false;
            //Release: We need to prevent the reader threads reading this after it was freed. So we need to increment the generation first using release, then reset.
            atomic_ref(slot.generation).fetch_add(1, memory_order_release);
            slot.data.reset();
            freeList.emplace_back(index);
            return true;
        }
    };
}