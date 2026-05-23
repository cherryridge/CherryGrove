#pragma once
#include <chrono>

namespace Util {
    typedef uint64_t u64;
    using std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::time_point, std::chrono::milliseconds, std::chrono::microseconds, std::chrono::nanoseconds, std::chrono::seconds;

    using TimePoint = time_point<steady_clock>;
    using TimeNs = std::chrono::nanoseconds;
    using TimeUs = std::chrono::microseconds;
    using TimeMs = std::chrono::milliseconds;
    using TimeS = std::chrono::seconds;
    using TimeM = std::chrono::minutes;
    using TimeH = std::chrono::hours;

    [[nodiscard]] inline TimePoint now() noexcept { return steady_clock::now(); }

    [[nodiscard]] inline u64 timeDiffNs(TimePoint start, TimePoint end) noexcept {
        return static_cast<u64>(duration_cast<TimeNs>(end - start).count());
    }

    [[nodiscard]] inline u64 timeDiffUs(TimePoint start, TimePoint end) noexcept {
        return static_cast<u64>(duration_cast<TimeUs>(end - start).count());
    }

    [[nodiscard]] inline u64 timeDiffMs(TimePoint start, TimePoint end) noexcept {
        return static_cast<u64>(duration_cast<TimeMs>(end - start).count());
    }

    [[nodiscard]] inline u64 timeDiffS(TimePoint start, TimePoint end) noexcept {
        return static_cast<u64>(duration_cast<TimeS>(end - start).count());
    }

    [[nodiscard]] inline u64 timeDiffM(TimePoint start, TimePoint end) noexcept {
        return static_cast<u64>(duration_cast<TimeM>(end - start).count());
    }

    [[nodiscard]] inline u64 timeDiffH(TimePoint start, TimePoint end) noexcept {
        return static_cast<u64>(duration_cast<TimeH>(end - start).count());
    }

    [[nodiscard]] inline TimeNs asNs(u64 time) noexcept { return TimeNs(time); }

    [[nodiscard]] inline TimeUs asUs(u64 time) noexcept { return TimeUs(time); }

    [[nodiscard]] inline TimeMs asMs(u64 time) noexcept { return TimeMs(time); }

    [[nodiscard]] inline TimeS asS(u64 time) noexcept { return TimeS(time); }

    [[nodiscard]] inline TimeM asM(u64 time) noexcept { return TimeM(time); }

    [[nodiscard]] inline TimeH asH(u64 time) noexcept { return TimeH(time); }
}