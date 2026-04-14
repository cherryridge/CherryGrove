#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
    #define CG_PLATFORM_WINDOWS 1
    //We must only rely on this windows.h header because it's too awful we need to undefine a lot of macros after including it.
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <cstdio> // IWYU pragma: keep
    #include <windows.h>  // IWYU pragma: keep
    #include <fcntl.h>  // IWYU pragma: keep
    #include <io.h>  // IWYU pragma: keep
    #undef CONST
    #undef FAR
    #undef NEAR
    #undef min
    #undef max
#elif defined(__linux__)
    #define CG_PLATFORM_LINUX 1
    #include <fcntl.h>  // IWYU pragma: keep
    #include <semaphore.h>  // IWYU pragma: keep
    #include <unistd.h>  // IWYU pragma: keep
    #include <climits>  // IWYU pragma: keep
#elif defined(__ANDROID__)
    #define CG_PLATFORM_ANDROID 1
    #include <unistd.h>  // IWYU pragma: keep
#elif defined(__APPLE__)
    #include <TargetConditionals.h>  // IWYU pragma: keep
    #include <mach-o/dyld.h>  // IWYU pragma: keep
    #include <unistd.h>  // IWYU pragma: keep
    #include <pthread.h>  // IWYU pragma: keep
    #if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
        #define CG_PLATFORM_IOS 1
    #elif TARGET_OS_MAC
        #define CG_PLATFORM_MACOS 1
        #include <fcntl.h>  // IWYU pragma: keep
        #include <semaphore.h>  // IWYU pragma: keep
        #include <objc/message.h>  // IWYU pragma: keep
        #include <objc/runtime.h>  // IWYU pragma: keep
    #else
        #error "Unknown Apple platform"
    #endif
#else
    #error "Unknown platform"
#endif