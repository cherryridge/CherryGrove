#pragma once
#include <cstdint>

#include "platform.hpp"

namespace Util {
    typedef uint64_t u64;

    [[nodiscard]] inline u64 getThreadId() noexcept {
    #if CG_PLATFORM_WINDOWS
        return static_cast<u64>(GetCurrentThreadId());
    #elif CG_PLATFORM_LINUX
        return static_cast<u64>(syscall(SYS_gettid));
    #elif CG_PLATFORM_MACOS
        u64 tid;
        pthread_threadid_np(nullptr, &tid);
        return tid;
    #elif CG_PLATFORM_ANDROID
        return static_cast<u64>(syscall(SYS_gettid));
    #elif CG_PLATFORM_IOS
        u64 tid;
        pthread_threadid_np(nullptr, &tid);
        return tid;
    #endif
    }
}