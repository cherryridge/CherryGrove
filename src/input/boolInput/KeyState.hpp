#pragma once
#include <ostream>

#include "../../debug/Logger.hpp"
#include "../../simulation/Time.hpp"

namespace InputHandler::BoolInput {
    typedef uint64_t u64;
    using Simulation::TimePoint, Simulation::TimeUnit, std::ostream;

    //Dual-layer per-key state tracker.
    //Physical layer: Exact hardware state for edge-triggered events and double-tap detection.
    //Virtual layer: "Sticky" state with TTL compensation for chord/combo tolerance.
    //
    //Design rationale (from architecture discussion):
    //  - On Down: physical=DOWN, virtual=DOWN, timestamps updated (re-press extends virtual).
    //  - On Up: physical=UP immediately; virtual stays DOWN until minTTL expires.
    //  - On Update tick: expire virtual states whose TTL has passed.
    //  - Double-tap: compare current press time with previous press time in physical layer.
    struct KeyState {
        bool isPhysicalDown{false}, isVirtualDown{false};
        //Timestamp of the most recent physical key-down event.
        TimePoint lastPressTime{TimePoint::min()};
        //Timestamp of the most recent physical key-up event.
        TimePoint lastReleaseTime{TimePoint::min()};
        //Timestamp of the *second to last* physical key-down event (history for multi-tap detection).
        TimePoint prevPressTime{TimePoint::min()};
        //The TimePoint at which the current virtual hold began (set on press, extended on re-press).
        TimePoint virtualPressStart{TimePoint::min()};
        //Consecutive tap count. Reset to 1 when the gap between presses exceeds repeatTapGap.
        //0 means no press has been registered yet.
        u64 tapCount{0};

        //Call when a physical key-down event arrives.
        void onPhysicalPress(TimePoint at) noexcept {
            //Tap counting: if the gap from the last press is within threshold, increment; otherwise reset to 1.
            if (tapCount > 0 && lastPressTime != TimePoint::min() && (at - lastPressTime) <= repeatTapGap) tapCount++;
            else tapCount = 1;
            prevPressTime = lastPressTime;
            lastPressTime = at;
            isPhysicalDown = true;
            if (!isVirtualDown) virtualPressStart = at;
            isVirtualDown = true;
        }

        void onPhysicalRelease(TimePoint at) noexcept {
            isPhysicalDown = false;
            lastReleaseTime = at;
        }

        void tryExpireVirtual(TimePoint now) noexcept {
            if (!isPhysicalDown && isVirtualDown && now - lastReleaseTime >= comboMinTTL) isVirtualDown = false;
        }

        void reset() noexcept {
            isPhysicalDown = false;
            isVirtualDown = false;
            lastPressTime = TimePoint::min();
            lastReleaseTime = TimePoint::min();
            prevPressTime = TimePoint::min();
            virtualPressStart = TimePoint::min();
            tapCount = 0;
        }

    private:
        inline static TimeUnit repeatTapGap, comboMinTTL;
    public:
        [[nodiscard]] static TimeUnit getRepeatTapGap() noexcept { return repeatTapGap; }
        [[nodiscard]] static TimeUnit getComboMinTTL() noexcept { return comboMinTTL; }
        static void setRepeatTapGap(TimeUnit gap) noexcept { repeatTapGap = gap; }
        static void setComboMinTTL(TimeUnit ttl) noexcept { comboMinTTL = ttl; }

        friend ostream& operator<<(ostream& os, const KeyState& s) noexcept {
            os << "KeyState(phys=" << s.isPhysicalDown << " virt=" << s.isVirtualDown << ")";
            return os;
        }
        friend Logger::Logger& operator<<(Logger::Logger& logger, const KeyState& s) noexcept {
            logger << "KeyState(phys=" << s.isPhysicalDown << " virt=" << s.isVirtualDown << ")";
            return logger;
        }
    };
}