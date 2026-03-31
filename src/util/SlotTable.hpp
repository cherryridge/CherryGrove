#pragma once
#include <vector>
#include <ostream>

#include "../debug/Logger.hpp"
#include "concepts.hpp"

namespace Util {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::vector, std::forward, std::ostream;

    struct GenerationalHandle {
    private:
        u32 generation{0}, index{0};

    public:
        [[nodiscard]] constexpr GenerationalHandle() noexcept = default;
        [[nodiscard]] constexpr explicit GenerationalHandle(u64 raw) noexcept : generation(static_cast<u32>(raw >> 32)), index(static_cast<u32>(raw & 0xFFFFFFFF)) {}
        [[nodiscard]] constexpr GenerationalHandle(u32 generation, u32 index) noexcept : generation(generation), index(index) {}
        [[nodiscard]] constexpr GenerationalHandle(const GenerationalHandle&) noexcept = default;
        [[nodiscard]] constexpr GenerationalHandle(GenerationalHandle&&) noexcept = default;
        constexpr GenerationalHandle& operator=(const GenerationalHandle&) noexcept = default;
        constexpr GenerationalHandle& operator=(GenerationalHandle&&) noexcept = default;

        [[nodiscard]] constexpr u32 getGeneration() const noexcept { return generation; }
        [[nodiscard]] constexpr u32 getIndex() const noexcept { return index; }

        [[nodiscard]] constexpr bool operator==(const GenerationalHandle& other) const noexcept { return generation == other.generation && index == other.index; }
        [[nodiscard]] constexpr bool operator!=(const GenerationalHandle& other) const noexcept { return generation != other.generation || index != other.index; }
        [[nodiscard]] constexpr explicit operator u64() const noexcept { return (static_cast<u64>(generation) << 32) | index; }

        friend Logger::Logger& operator<<(Logger::Logger& os, const GenerationalHandle& data) noexcept {
            os << "GenerationalHandle (idx " << data.index << ", gen " << data.generation << ")";
            return os;
        }

        friend ostream& operator<<(ostream& os, const GenerationalHandle& data) noexcept {
            os << "GenerationalHandle (idx " << data.index << ", gen " << data.generation << ")";
            return os;
        }
    };

    template <typename EntryType, typename HandleType = GenerationalHandle> requires EqualStrict<HandleType, GenerationalHandle> || DistinctHandleOf<HandleType, GenerationalHandle>
    struct SlotTable {
        struct Entry {
            EntryType data;
            //0 is reserved for tombstone.
            //Odd is occupied, even is free, because we're using copy assignment operator on emplace and do not destruct EntryType on destroy.
            u32 generation{1};
        };

        vector<Entry> storage;
        vector<u32> freeList;

        [[nodiscard]] SlotTable() noexcept = default;
        [[nodiscard]] SlotTable(u8 originSizeMagnitude) noexcept { storage.reserve(1ull << originSizeMagnitude); }

    private:
        [[nodiscard]] constexpr static GenerationalHandle getHandle(HandleType handle) noexcept {
            if constexpr (EqualStrict<HandleType, GenerationalHandle>) return handle;
            else return handle.value;
        }

        [[nodiscard]] constexpr static HandleType wrapHandle(GenerationalHandle handle) noexcept {
            if constexpr (EqualStrict<HandleType, GenerationalHandle>) return handle;
            else return HandleType{
                .value = handle
            };
        }

    public:
        template <typename... Args>
        [[nodiscard]] HandleType emplace(Args&&... args) noexcept {
            if (freeList.empty()) {
                const u32 index = static_cast<u32>(storage.size());
                storage.emplace_back(EntryType(forward<Args>(args)...), 1);
                return wrapHandle(GenerationalHandle{1, index});
            }
            else {
                const u32 index = freeList.back();
                freeList.pop_back();
                auto& slot = storage[index];
                slot.data = EntryType(forward<Args>(args)...);
                slot.generation++;
                return wrapHandle(GenerationalHandle{slot.generation, index});
            }
        }

        [[nodiscard]] const EntryType* get(HandleType handle) const noexcept {
            const GenerationalHandle genHandle = getHandle(handle);
            const u32 generation = genHandle.getGeneration(), index = genHandle.getIndex();
            if (index >= storage.size()) return nullptr;
            const auto& slot = storage[index];
            if (slot.generation != generation) return nullptr;
            return &slot.data;
        }

        [[nodiscard]] EntryType* get(HandleType handle) noexcept {
            const GenerationalHandle genHandle = getHandle(handle);
            const u32 generation = genHandle.getGeneration(), index = genHandle.getIndex();
            if (index >= storage.size()) return nullptr;
            auto& slot = storage[index];
            if (slot.generation != generation) return nullptr;
            return &slot.data;
        }

        [[nodiscard]] bool contains(HandleType handle) const noexcept {
            const GenerationalHandle genHandle = getHandle(handle);
            const u32 generation = genHandle.getGeneration(), index = genHandle.getIndex();
            if (index >= storage.size()) return false;
            return storage[index].generation == generation;
        }

        //Returns invalid handle if slot is invalid or free.
        [[nodiscard]] HandleType getCurrentHandle(u32 index) const noexcept {
            if (index >= storage.size() || (storage[index].generation & 1) == 0) return wrapHandle(GenerationalHandle{0, index});
            return wrapHandle(GenerationalHandle{storage[index].generation, index});
        }

        [[nodiscard]] bool is_empty() const noexcept { return freeList.size() + storage.size() == 0; }
        [[nodiscard]] size_t size() const noexcept { return storage.size() - freeList.size(); }

        [[nodiscard]] bool destroy(HandleType handle) noexcept {
            const GenerationalHandle genHandle = getHandle(handle);
            const u32 generation = genHandle.getGeneration(), index = genHandle.getIndex();
            if (index >= storage.size()) return false;
            auto& slot = storage[index];
            if (slot.generation != generation) return false;
            slot.generation++;
            freeList.emplace_back(index);
            return true;
        }
    };
}