#pragma once
#include <atomic>
#include <iostream>
#include <ostream>
#include <thread>
#include <type_traits>
#include <utility>

namespace Debug {
    typedef uint64_t u64;
    using std::cout, std::cerr, std::atomic_flag, std::ostream, std::memory_order_acquire, std::memory_order_release, std::forward, std::is_function_v, std::this_thread::yield;

    struct AtomicizedOutput {
    private:
        atomic_flag flag{};
        ostream& dest;

        void lock() noexcept { for (u64 i = 0; flag.test_and_set(memory_order_acquire); i++) if ((i & 0xFF) == 0) yield(); }
        void unlock() noexcept { flag.clear(memory_order_release); }

    public:
        explicit AtomicizedOutput(ostream& dest) noexcept : dest(dest) {}
        AtomicizedOutput(const AtomicizedOutput&) = delete;
        AtomicizedOutput& operator=(const AtomicizedOutput&) = delete;

        template <typename T> requires (!is_function_v<T>)
        AtomicizedOutput& operator<<(T&& value) noexcept {
            lock();
            dest << value;
            unlock();
            return *this;
        }

        void flush() noexcept {
            lock();
            dest.flush();
            unlock();
        }

        template <typename... Ts>
        AtomicizedOutput& operator()(Ts&&... values) noexcept {
            lock();
            ((dest << forward<Ts>(values)), ...);
            unlock();
            return *this;
        }
    };

    inline AtomicizedOutput atomicCout(cout), atomicCerr(cerr);
}