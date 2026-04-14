#pragma once

#include "../../util/os/platform.hpp"

#if CG_PLATFORM_WINDOWS
    #include "Windows.hpp" // IWYU pragma: export
#elif CG_PLATFORM_LINUX
    #include "Linux.hpp" // IWYU pragma: export
#elif CG_PLATFORM_MACOS
    #include "MacOS.hpp" // IWYU pragma: export
#endif