#pragma once
#include <cstdint>

#include "../util/json/helpers.hpp"

namespace Debug {
    typedef uint8_t u8;

    enum struct LoggingMode : u8 { Stdout, Separate, File };
}

GLAZE_ENUM_START(Debug::LoggingMode)
    GLAZE_ENUM("stdout", Stdout),
    GLAZE_ENUM("separate", Separate),
    GLAZE_ENUM("file", File)
GLAZE_ENUM_END