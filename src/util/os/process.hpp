#pragma once
#include <cstdint>

#include "platform.hpp"

namespace Util::OS {
    typedef uint32_t u32;

    namespace detail {
        inline u32 cachedId{0};
    }

    [[nodiscard]] inline u32 getProcessId() noexcept {
        if (detail::cachedId == 0) {
        #if CG_PLATFORM_WINDOWS
            detail::cachedId = static_cast<u32>(GetCurrentProcessId());
        #else
            detail::cachedId = static_cast<u32>(getpid());
        #endif
        }
        return detail::cachedId;
    }
}