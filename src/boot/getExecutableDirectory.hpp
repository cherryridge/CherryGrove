#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include "../debug/Fatal.hpp"
#include "../util/os/platform.hpp"

namespace Boot {
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::filesystem::path, std::wstring, std::string, std::vector, std::max;

    //note: Pre-logger function
    //note: Pure function
    //It may exit on failure. We also can't recover.
    [[nodiscard]] inline path getExecutableDirectory() noexcept {
        #if CG_PLATFORM_WINDOWS
            wstring exePathStr;
            constexpr DWORD maxAttempts = 10;
            DWORD attempts = 0;
            for(DWORD cap = 260; attempts < maxAttempts; cap *= 2, attempts++) {
                if (cap > 32768) Fatal::exit(Fatal::FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH);
                exePathStr.resize(cap);
                const DWORD len = GetModuleFileNameW(NULL, exePathStr.data(), cap);
                if (len == 0) Fatal::exit(Fatal::FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH);
                if (len < cap - 1) {
                    exePathStr.resize(len);
                    break;
                }
            }
            if (attempts >= maxAttempts) Fatal::exit(Fatal::FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH);
            path exeDict = path(exePathStr).parent_path();
            if (exeDict.empty()) Fatal::exit(Fatal::FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH);
            return exeDict;
        #elif CG_PLATFORM_LINUX
            vector<char> buffer(PATH_MAX + 1);
            ssize_t len = readlink("/proc/self/exe", buffer.data(), PATH_MAX);
            if (len == PATH_MAX) for(ssize_t cap = PATH_MAX * 2; cap <= PATH_MAX * 8; cap *= 2) {
                buffer.resize(cap + 1);
                len = readlink("/proc/self/exe", buffer.data(), cap);
                if (len < cap && len > 0) break;
                if (cap >= PATH_MAX * 16) Fatal::exit(Fatal::FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH);
            }
            if (len <= 0) Fatal::exit(Fatal::FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH);
            buffer[len] = '\0';
            path exeDict = path(string(buffer.data())).parent_path();
            if (exeDict.empty()) Fatal::exit(Fatal::FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH);
            return exeDict;
        #elif CG_PLATFORM_MACOS
            u32 cap = 256, attempts = 0;
            constexpr u32 maxAttempts = 10;
            vector<char> buf(cap);
            for (; attempts < maxAttempts; attempts++) {
                i32 rc = _NSGetExecutablePath(buf.data(), &cap);
                if (rc == 0) break;
                if (cap > PATH_MAX * 8) Fatal::exit(Fatal::FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH);
                buf.resize(cap);
            }
            if (attempts >= maxAttempts) Fatal::exit(Fatal::FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH);
            vector<char> resolved(max(static_cast<size_t>(PATH_MAX * 2), static_cast<size_t>(cap)) + 1);
            if (realpath(buf.data(), resolved.data())) {
                path exeDict = path(string(resolved.data())).parent_path();
                if (!exeDict.empty()) return exeDict;
            }
            path exeDict = path(string(buf.data())).parent_path();
            if (exeDict.empty()) Fatal::exit(Fatal::FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH);
            return exeDict;
        #elif CG_PLATFORM_ANDROID
            Fatal::exit(Fatal::MISC_UNSUPPORTED_PLATFORM);
        #elif CG_PLATFORM_IOS
            Fatal::exit(Fatal::MISC_UNSUPPORTED_PLATFORM);
        #endif
    }
}