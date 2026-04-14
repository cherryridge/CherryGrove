#pragma once
#include <cstring>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "../debug/Fatal.hpp"
#include "../util/os/filesystem.hpp"
#include "../util/os/platform.hpp"
#include "../util/os/process.hpp"
#include "focus/Focus.hpp"

namespace Boot {
    typedef uint8_t u8;
    typedef int32_t i32;
    typedef uint32_t u32;
    using std::string, std::move, std::string_view, std::filesystem::current_path, std::filesystem::remove, std::span, std::vector;

    struct SessionLock {
        SessionLock() = default;

        [[nodiscard]] SessionLock(const string_view fileName) noexcept {
            lockFilePath = (current_path() / fileName).string();

        #if CG_PLATFORM_WINDOWS
        {
            lockFile = CreateFileA(lockFilePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (lockFile != INVALID_HANDLE_VALUE) {
                bool locked = LockFile(lockFile, 0, 0, 1, 0);
                if (!locked) goto invokeExistingInstance;
            }
            else goto invokeExistingInstance;
        }
        #else
        {
            lockFile = open(lockFilePath.c_str(), O_CREAT | O_RDWR, 0666);
            if (lockFile >= 0) {
                bool locked = lockf(lockFile, F_TLOCK, 0) == 0;
                if (!locked) goto invokeExistingInstance;
            }
            else goto invokeExistingInstance;
        }
        #endif
            Focus::connect(lockFilePath);
            if (!writePidToLockFile()) Fatal::exit(Fatal::FILESYSTEM_NO_WRITE_PERMISSION);
            return;

        invokeExistingInstance: {
            vector<u8> fileData;
            if (Util::OS::readFile<false>(lockFilePath, fileData) && fileData.size() >= sizeof(u32)) {
                u32 pid;
                memcpy(&pid, fileData.data(), sizeof(u32));
            #if CG_PLATFORM_WINDOWS
                AllowSetForegroundWindow(pid);
            #endif
            }
            Focus::sendMessage(lockFilePath);
            Fatal::exit(Fatal::BOOT_MULTIPLE_INSTANCES);
        }
        }

        SessionLock(const SessionLock&) = delete;
        SessionLock(SessionLock&& other) noexcept :
            lockFilePath(move(other.lockFilePath)),
            lockFile(other.lockFile) {
            other.lockFile = invalidLockFile();
            other.lockFilePath.clear();
        }
        SessionLock& operator=(const SessionLock&) = delete;
        SessionLock& operator=(SessionLock&& other) noexcept {
            if (this == &other) return *this;
            releaseLock();
            lockFilePath = move(other.lockFilePath);
            lockFile = other.lockFile;
            other.lockFile = invalidLockFile();
            other.lockFilePath.clear();
            return *this;
        }

        ~SessionLock() { releaseLock(); }

    private:
    #if CG_PLATFORM_WINDOWS
        [[nodiscard]] static HANDLE invalidLockFile() noexcept { return INVALID_HANDLE_VALUE; }
    #else
        [[nodiscard]] static constexpr i32 invalidLockFile() noexcept { return -1; }
    #endif

        string lockFilePath;

    #if CG_PLATFORM_WINDOWS
        HANDLE lockFile{INVALID_HANDLE_VALUE};
    #else
        i32 lockFile{-1};
    #endif

        [[nodiscard]] bool writePidToLockFile() noexcept {
            const u32 pid = Util::OS::getProcessId();
            const span<const u8> pidBytes(reinterpret_cast<const u8*>(&pid), sizeof(pid));

        #if CG_PLATFORM_WINDOWS
            LARGE_INTEGER offset{};
            if (!SetFilePointerEx(lockFile, offset, nullptr, FILE_BEGIN)) return false;
            DWORD written = 0;
            if (!WriteFile(lockFile, pidBytes.data(), static_cast<DWORD>(pidBytes.size()), &written, nullptr)) return false;
            if (written != static_cast<DWORD>(pidBytes.size())) return false;
            if (!SetEndOfFile(lockFile)) return false;
            return FlushFileBuffers(lockFile) != 0;
        #else
            if (ftruncate(lockFile, 0) != 0) return false;
            if (lseek(lockFile, 0, SEEK_SET) < 0) return false;
            size_t totalWritten = 0;
            while (totalWritten < pidBytes.size()) {
                const auto written = write(lockFile, pidBytes.data() + totalWritten, pidBytes.size() - totalWritten);
                if (written <= 0) return false;
                totalWritten += static_cast<size_t>(written);
            }
            return true;
        #endif
        }

        void releaseLock() noexcept {
            if (lockFile == invalidLockFile()) {
                lockFilePath.clear();
                return;
            }

            Focus::disconnect();

        #if CG_PLATFORM_WINDOWS
            UnlockFile(lockFile, 0, 0, 1, 0);
            CloseHandle(lockFile);
        #else
            lockf(lockFile, F_ULOCK, 0);
            close(lockFile);
        #endif
            lockFile = invalidLockFile();

            if (!lockFilePath.empty()) {
                remove(lockFilePath);
                lockFilePath.clear();
            }
        }
    };
}