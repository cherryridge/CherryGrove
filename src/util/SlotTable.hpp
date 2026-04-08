#pragma once
#include <compare>
#include <memory>
#include <vector>
#include <ostream>

#include "../debug/Logger.hpp"
#include "concepts.hpp"

namespace Util {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::vector, std::forward, std::ostream, std::strong_ordering, std::construct_at, std::destroy_at;

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

        constexpr bool operator==(const GenerationalHandle& other) const noexcept { return generation == other.generation && index == other.index; }
        constexpr bool operator!=(const GenerationalHandle& other) const noexcept { return !(*this == other); }
        constexpr strong_ordering operator<=>(const GenerationalHandle& other) const noexcept {
            if (index <=> other.index != strong_ordering::equal) return index <=> other.index;
            return generation <=> other.generation;
        }

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

    #define MAKE_DISTINCT_HANDLE(type)                                                                       \
    struct type {                                                                                            \
        Util::GenerationalHandle value;                                                                      \
        bool operator==(const type& other) const noexcept { return value == other.value; }                   \
        std::strong_ordering operator<=>(const type& other) const noexcept { return value <=> other.value; } \
    };

    template <typename EntryType, typename HandleType = GenerationalHandle> requires EqualStrict<HandleType, GenerationalHandle> || DistinctHandleOf<HandleType, GenerationalHandle>
    struct SlotTable {
    private:
        struct Entry {
            EntryType data;
            //0 is reserved for tombstone. Odd is occupied, even is free.
            //We're using copy assignment operator to actually destroy things on emplace instead of destroying them on destroy though. This has nothing to do with generation counting.
            //The default value of `1` makes every slot start as occupied, which is a good default. But you can safely ignore it, since we will assign a value to generation on every path.
            u32 generation{1};
        };

        vector<Entry> storage;
        vector<u32> freeList;

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

        [[nodiscard]] size_t nextOccupiedIndex(size_t index) const noexcept {
            while (index < storage.size() && (storage[index].generation & 1) == 0) index++;
            return index;
        }

    public:
        [[nodiscard]] SlotTable() noexcept = default;
        [[nodiscard]] SlotTable(u8 originSizeMagnitude) noexcept { storage.reserve(1ull << originSizeMagnitude); }

        template <typename... Args>
        [[nodiscard]] HandleType emplace(Args&&... args) noexcept {
            if (freeList.empty()) {
                const u32 index = static_cast<u32>(storage.size());
                storage.push_back(Entry{EntryType(forward<Args>(args)...), 1});
                return wrapHandle(GenerationalHandle{1, index});
            }
            else {
                const u32 index = freeList.back();
                freeList.pop_back();
                auto& slot = storage[index];
                destroy_at(&slot.data);
                construct_at(&slot.data, forward<Args>(args)...);
                slot.generation++;
                return wrapHandle(GenerationalHandle{slot.generation, index});
            }
        }

        [[nodiscard]] const EntryType* get(HandleType handle) const noexcept {
            const GenerationalHandle genHandle = getHandle(handle);
            const u32 generation = genHandle.getGeneration(), index = genHandle.getIndex();
            if (index >= storage.size() || (generation & 1) == 0) return nullptr;
            const auto& slot = storage[index];
            if (slot.generation != generation) return nullptr;
            return &slot.data;
        }

        [[nodiscard]] EntryType* get(HandleType handle) noexcept {
            const GenerationalHandle genHandle = getHandle(handle);
            const u32 generation = genHandle.getGeneration(), index = genHandle.getIndex();
            if (index >= storage.size() || (generation & 1) == 0) return nullptr;
            auto& slot = storage[index];
            if (slot.generation != generation) return nullptr;
            return &slot.data;
        }

        [[nodiscard]] bool contains(HandleType handle) const noexcept {
            const GenerationalHandle genHandle = getHandle(handle);
            const u32 generation = genHandle.getGeneration(), index = genHandle.getIndex();
            if (index >= storage.size() || (generation & 1) == 0) return false;
            return storage[index].generation == generation;
        }

        //This is unnecessary since the default constructor of `HandleType` should give us a canonical invalid handle with (generation = 0, index = 0).
        //[[nodiscard]] HandleType getInvalidHandle() const noexcept { return wrapHandle(GenerationalHandle{0, 0}); }

        //Returns canonical invalid handle (generation 0) if slot is invalid or free, regardless of the current generation.
        [[nodiscard]] HandleType getCurrentHandle(u32 index) const noexcept {
            if (index >= storage.size() || (storage[index].generation & 1) == 0) return wrapHandle(GenerationalHandle{0, index});
            return wrapHandle(GenerationalHandle{storage[index].generation, index});
        }

        [[nodiscard]] bool isNew() const noexcept { return freeList.size() + storage.size() == 0; }
        [[nodiscard]] bool isEmpty() const noexcept { return freeList.size() == storage.size(); }
        [[nodiscard]] size_t size() const noexcept { return storage.size() - freeList.size(); }

        [[nodiscard]] bool destroy(HandleType handle) noexcept {
            const GenerationalHandle genHandle = getHandle(handle);
            const u32 generation = genHandle.getGeneration(), index = genHandle.getIndex();
            if (index >= storage.size() || (generation & 1) == 0) return false;
            auto& slot = storage[index];
            if (slot.generation != generation) return false;
            slot.generation++;
            freeList.emplace_back(index);
            return true;
        }

        struct Iterator {
            SlotTable<EntryType, HandleType>* table{nullptr};
            size_t index{0};

            Iterator() noexcept = default;
            Iterator(SlotTable<EntryType, HandleType>* table, size_t index) noexcept : table(table), index(index) {}

            EntryType& operator*() const noexcept {
                return *(
                    table->get(
                        table->wrapHandle(GenerationalHandle{table->storage[index].generation, static_cast<u32>(index)})
                    )
                );
            }

            EntryType* operator->() const noexcept {
                return table->get(
                    table->wrapHandle(GenerationalHandle{table->storage[index].generation, static_cast<u32>(index)})
                );
            }

            Iterator& operator++() noexcept {
                index = table->nextOccupiedIndex(index + 1);
                return *this;
            }

            Iterator operator++(int) noexcept {
                Iterator temp = *this;
                ++(*this);
                return temp;
            }

            bool operator==(const Iterator& other) const noexcept { return table == other.table && index == other.index; }
            bool operator!=(const Iterator& other) const noexcept { return !(*this == other); }
        };

        struct ConstIterator {
            const SlotTable<EntryType, HandleType>* table{nullptr};
            size_t index{0};

            ConstIterator() noexcept = default;
            ConstIterator(const SlotTable<EntryType, HandleType>* table, size_t index) noexcept : table(table), index(index) {}
            ConstIterator(const Iterator& other) noexcept : table(other.table), index(other.index) {}

            const EntryType& operator*() const noexcept {
                return *(
                    table->get(
                        table->wrapHandle(GenerationalHandle{table->storage[index].generation, static_cast<u32>(index)})
                    )
                );
            }

            const EntryType* operator->() const noexcept {
                return table->get(
                    table->wrapHandle(GenerationalHandle{table->storage[index].generation, static_cast<u32>(index)})
                );
            }

            ConstIterator& operator++() noexcept {
                index = table->nextOccupiedIndex(index + 1);
                return *this;
            }

            ConstIterator operator++(int) noexcept {
                ConstIterator temp = *this;
                ++(*this);
                return temp;
            }

            bool operator==(const ConstIterator& other) const noexcept { return table == other.table && index == other.index; }
            bool operator!=(const ConstIterator& other) const noexcept { return !(*this == other); }
        };

        Iterator begin() noexcept { return Iterator{this, nextOccupiedIndex(0)}; }
        Iterator end() noexcept { return Iterator{this, storage.size()}; }

        ConstIterator begin() const noexcept { return ConstIterator{this, nextOccupiedIndex(0)}; }
        ConstIterator end() const noexcept { return ConstIterator{this, storage.size()}; }
        ConstIterator cbegin() const noexcept { return begin(); }
        ConstIterator cend() const noexcept { return end(); }
    };
}
