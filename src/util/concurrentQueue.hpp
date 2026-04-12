#pragma once
#include <atomic>
#include <new>

namespace Util {
    typedef uint8_t u8;
    typedef uint64_t u64;
    using std::atomic, std::memory_order_relaxed, std::memory_order_acquire, std::memory_order_release, std::move, std::forward, std::align_val_t;

    template <typename EntryType>
    struct SPSCQueue {
    private:
        struct Entry {
            EntryType data;
            //Whether data has been pushed and is ready for consumer to consume.
            atomic<bool> ready{false};
        };

        Entry* storage;
        const u64 size;
        u64 head{0}, tail{0};

    public:
        //Defaults to 1024 entries.
        [[nodiscard]] explicit SPSCQueue(u8 sizeMagnitude = 10) noexcept : size(1ull << sizeMagnitude) {
            storage = static_cast<Entry*>(operator new(sizeof(Entry) * size, align_val_t(alignof(Entry))));
            for (u64 i = 0; i < size; i++) new (&storage[i]) Entry{};
        }

        template <typename T = EntryType>
        void enqueue(T&& data) noexcept {
            Entry& entry = storage[tail & (size - 1)];
            //The tail may loop back and clash with the head. To prevent overriding data, we need to wait for `ready` to become `false`.
            entry.ready.wait(true, memory_order_acquire);
            entry.data = forward<T>(data);
            entry.ready.store(true, memory_order_release);
            tail++;
        }

        [[nodiscard]] const EntryType* peek() noexcept {
            Entry& entry = storage[head & (size - 1)];
            //Empty
            if (!entry.ready.load(memory_order_acquire)) return nullptr;
            return &entry.data;
        }

        [[nodiscard]] bool dequeue(EntryType& data) noexcept {
            Entry& entry = storage[head & (size - 1)];
            //Empty
            if (!entry.ready.load(memory_order_acquire)) return false;
            data = move(entry.data);
            entry.ready.store(false, memory_order_release);
            //Only notifying one thread is enough.
            entry.ready.notify_one();
            head++;
            return true;
        }

        //Warning: This function is not thread-safe. Only call it when no other threads are accessing the queue.
        void clear() noexcept {
            head = 0;
            tail = 0;
            for (u64 i = 0; i < size; i++) {
                storage[i].ready.store(false, memory_order_relaxed);
                storage[i].ready.notify_all();
            }
        }

        ~SPSCQueue() {
            for (u64 i = 0; i < size; i++) storage[i].~Entry();
            operator delete(storage, align_val_t(alignof(Entry)));
        }
    };

    template <typename EntryType>
    struct MPSCQueue {
    private:
        struct Entry {
            EntryType data;
            //Whether data has been pushed and is ready for consumer to consume.
            atomic<bool> ready{false};
        };

        Entry* storage;
        const u64 size;
        u64 head{0};
        atomic<u64> tail{0};

    public:
        //Defaults to 1024 entries.
        [[nodiscard]] explicit MPSCQueue(u8 sizeMagnitude = 10) noexcept : size(1ull << sizeMagnitude) {
            storage = static_cast<Entry*>(operator new(sizeof(Entry) * size, align_val_t(alignof(Entry))));
            for (u64 i = 0; i < size; i++) new (&storage[i]) Entry{};
        }

        template <typename T = EntryType>
        void enqueue(T&& data) noexcept {
            u64 pos = tail.fetch_add(1, memory_order_acquire);
            Entry& entry = storage[pos & (size - 1)];
            //The tail may loop back and clash with the head. To prevent overriding data, we need to wait for `ready` to become `false`.
            entry.ready.wait(true, memory_order_acquire);
            entry.data = forward<T>(data);
            entry.ready.store(true, memory_order_release);
        }

        [[nodiscard]] const EntryType* peek() noexcept {
            Entry& entry = storage[head & (size - 1)];
            //Empty
            if (!entry.ready.load(memory_order_acquire)) return nullptr;
            return &entry.data;
        }

        [[nodiscard]] bool dequeue(EntryType& data) noexcept {
            Entry& entry = storage[head & (size - 1)];
            //Empty
            if (!entry.ready.load(memory_order_acquire)) return false;
            data = move(entry.data);
            entry.ready.store(false, memory_order_release);
            //Only notifying one thread is enough.
            entry.ready.notify_one();
            head++;
            return true;
        }

        //Warning: This function is not thread-safe. Only call it when no other threads are accessing the queue.
        void clear() noexcept {
            head = 0;
            tail.store(0, memory_order_relaxed);
            for (u64 i = 0; i < size; i++) {
                storage[i].ready.store(false, memory_order_relaxed);
                storage[i].ready.notify_all();
            }
        }

        ~MPSCQueue() {
            for (u64 i = 0; i < size; i++) storage[i].~Entry();
            operator delete(storage, align_val_t(alignof(Entry)));
        }
    };

    template <typename EntryType>
    struct SPMCQueue {
    private:
        struct Entry {
            EntryType data;
            //Whether data has been pushed and is ready for consumer to consume.
            atomic<bool> ready{false};
        };

        Entry* storage;
        const u64 size;
        atomic<u64> head{0};
        u64 tail{0};

    public:
        //Defaults to 1024 entries.
        [[nodiscard]] explicit SPMCQueue(u8 sizeMagnitude = 10) noexcept : size(1ull << sizeMagnitude) {
            storage = static_cast<Entry*>(operator new(sizeof(Entry) * size, align_val_t(alignof(Entry))));
            for (u64 i = 0; i < size; i++) new (&storage[i]) Entry{};
        }

        template <typename T = EntryType>
        void enqueue(T&& data) noexcept {
            Entry& entry = storage[tail & (size - 1)];
            //The tail may loop back and clash with the head. To prevent overriding data, we need to wait for `ready` to become `false`.
            entry.ready.wait(true, memory_order_acquire);
            entry.data = forward<T>(data);
            entry.ready.store(true, memory_order_release);
            tail++;
        }

        //Just don't peek. It's not thread-safe at all across all consumer threads.

        [[nodiscard]] bool dequeue(EntryType& data) noexcept {
            u64 pos = head.fetch_add(1, memory_order_acquire);
            Entry& entry = storage[pos & (size - 1)];
            //Empty
            if (!entry.ready.load(memory_order_acquire)) return false;
            data = move(entry.data);
            entry.ready.store(false, memory_order_release);
            //Only notifying one thread is enough.
            entry.ready.notify_one();
            return true;
        }

        //Warning: This function is not thread-safe. Only call it when no other threads are accessing the queue.
        void clear() noexcept {
            head.store(0, memory_order_relaxed);
            tail = 0;
            for (u64 i = 0; i < size; i++) {
                storage[i].ready.store(false, memory_order_relaxed);
                storage[i].ready.notify_all();
            }
        }

        ~SPMCQueue() {
            for (u64 i = 0; i < size; i++) storage[i].~Entry();
            operator delete(storage, align_val_t(alignof(Entry)));
        }
    };
}