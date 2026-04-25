#pragma once
#include <chrono>

namespace Simulation {
    typedef uint64_t u64;
    using std::chrono::duration_cast, std::chrono::milliseconds, std::chrono::microseconds, std::chrono::steady_clock, std::chrono::time_point, std::chrono::nanoseconds;

    using TimePoint = time_point<steady_clock>;
    using TimeUnit = std::chrono::nanoseconds;

    [[nodiscard]] inline TimePoint now() noexcept { return steady_clock::now(); }

    [[nodiscard]] inline u64 timeDiff(TimePoint start, TimePoint end) noexcept {
        return static_cast<u64>(duration_cast<TimeUnit>(end - start).count());
    }

    [[nodiscard]] inline u64 timeDiffUs(TimePoint start, TimePoint end) noexcept {
        return static_cast<u64>(duration_cast<microseconds>(end - start).count());
    }

    [[nodiscard]] inline u64 timeDiffMs(TimePoint start, TimePoint end) noexcept {
        return static_cast<u64>(duration_cast<milliseconds>(end - start).count());
    }
}