#pragma once
#include <filesystem>
#include <iostream>
#include <string>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <fcntl.h>
    #include <unistd.h>
#endif

#include "../debug/Logger.hpp"
#include "../debug/Fatal.hpp"

namespace FileLock {
    typedef int32_t i32;
    using std::string, std::filesystem::path, std::filesystem::current_path, std::filesystem::remove;

    inline string lockFilePath;
    #ifdef _WIN32
        inline HANDLE lockFile;
    #else
        inline i32 lockFile;
    #endif // _WIN32

    inline bool addInstanceLock() noexcept {
        lockFilePath = current_path().string();
        lockFilePath += "\\running";
        #ifdef _WIN32
            lockFile = CreateFileA(lockFilePath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (lockFile != INVALID_HANDLE_VALUE) {
                bool locked = LockFile(lockFile, 0, 0, 1, 0);
                if (!locked) return false;
            }
            else return false;
        #else
            i32 lockFile = open(lockFilePath.c_str(), O_CREAT | O_RDWR, 0666);
            if (lockFile >= 0) {
                bool locked = lockf(lockFile, F_TLOCK, 0) == 0;
                if (!locked) return false;
            }
            else return false;
        #endif // _WIN32
        return true;
    }

    inline void removeInstanceLock() noexcept {
        #ifdef _WIN32
            UnlockFile(lockFile, 0, 0, 1, 0);
            CloseHandle(lockFile);
        #else
            lockf(lockFile, F_ULOCK, 0);
            close(lockFile);
        #endif // _WIN32
        remove(lockFilePath);
    }
}