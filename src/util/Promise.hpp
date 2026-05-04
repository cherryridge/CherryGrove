#pragma once
#include <atomic>

namespace Util {
    using std::atomic, std::memory_order_acquire, std::memory_order_release, std::memory_order_relaxed, std::move;

    //A single-shot promise/future for cross-thread result delivery.
    //Producer fulfills exactly once; consumer either `wait()`s (blocking, kernel park) or polls `tryTake()` (non-blocking) exactly once. Reuse via `reset()` is allowed but only once both sides have observed the previous round.
    //Non-copyable and non-movable: the fulfilling thread holds a raw pointer, so the storage location must be stable. Callers that hold many in-flight promises must use a stable-address container (e.g. `std::list<Promise<T>>` or a non-relocating pool); never `std::vector<Promise<T>>`, since reallocation would invalidate the pointer the producer is about to write through.
    template <typename T>
    struct Promise {
    private:
        T value{};
        atomic<bool> ready{false};

    public:
        [[nodiscard]] Promise() noexcept = default;
        Promise(const Promise&) = delete;
        Promise(Promise&&) = delete;
        Promise& operator=(const Promise&) = delete;
        Promise& operator=(Promise&&) = delete;
        ~Promise() = default;

        void fulfill(T v) noexcept {
            value = move(v);
            ready.store(true, memory_order_release);
            ready.notify_one();
        }

        [[nodiscard]] T wait() noexcept {
            ready.wait(false, memory_order_acquire);
            return move(value);
        }

        //Non-blocking consume. Returns true and moves the value into `out` if fulfilled; returns false and leaves `out` untouched otherwise.
        [[nodiscard]] bool tryTake(T& out) noexcept {
            if (!ready.load(memory_order_acquire)) return false;
            out = move(value);
            return true;
        }

        [[nodiscard]] bool isReady() const noexcept { return ready.load(memory_order_acquire); }

        //Only safe to call once both sides have observed the previous fulfillment.
        void reset() noexcept { ready.store(false, memory_order_relaxed); }
    };

    template <>
    struct Promise<void> {
    private:
        atomic<bool> ready{false};

    public:
        [[nodiscard]] Promise() noexcept = default;
        Promise(const Promise&) = delete;
        Promise(Promise&&) = delete;
        Promise& operator=(const Promise&) = delete;
        Promise& operator=(Promise&&) = delete;
        ~Promise() = default;

        void fulfill() noexcept {
            ready.store(true, memory_order_release);
            ready.notify_one();
        }

        void wait() noexcept { ready.wait(false, memory_order_acquire); }

        //Non-blocking check. Returns true iff fulfilled.
        [[nodiscard]] bool tryTake() noexcept { return ready.load(memory_order_acquire); }

        [[nodiscard]] bool isReady() const noexcept { return ready.load(memory_order_acquire); }

        void reset() noexcept { ready.store(false, memory_order_relaxed); }
    };
}