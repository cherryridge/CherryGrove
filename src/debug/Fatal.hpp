#pragma once
#include <cstdlib>

#include "dynamicLoggers.hpp"
//This is purely for convenience.
#include "fatalCodes.hpp"

namespace Debug {
    typedef int32_t i32;

    [[noreturn]] inline void exit(i32 code) noexcept {
        Debug::LOGGER_DYNAMIC_ERR("[Fatal] Exit code: ", code);
        std::exit(code);
    }
}