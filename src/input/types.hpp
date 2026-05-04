#pragma once
#include <cstdint>

namespace InputHandler {
    typedef uint8_t u8;
    typedef uint32_t u32;

    using EventControlFlags = u8;
    using ActionPriority = u32;
    using ActionID = u32;

    inline constexpr u8
        EVENT_CONTROL_FLAGS_STOP_PROMPTED = 1 << 0,
        EVENT_CONTROL_FLAGS_STOP_AT_LOWER_PRIORITY = 1 << 1,
        EVENT_CONTROL_FLAGS_STOP_IMMEDIATELY = 1 << 2;
}