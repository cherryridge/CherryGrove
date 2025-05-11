#pragma once
#include <string>
#include <cstdint>

namespace InputHandler {
    typedef uint8_t EventFlags;
    typedef uint32_t EventPriority;

    inline constexpr uint8_t EVENTFLAGS_CANCEL_PROMPTED = 1;
    inline constexpr uint8_t EVENTFLAGS_STOP_AFTER = 1 << 1;
    inline constexpr uint8_t EVENTFLAGS_STOP_IMMEDIATELY = 1 << 2;

    struct InputEventInfo {
        std::string nameSpace;
        std::string name;
        EventPriority priority;

        InputEventInfo(const std::string& nameSpace, const std::string& name, EventPriority priority) : nameSpace(nameSpace), name(name), priority(priority) {};
    };
}