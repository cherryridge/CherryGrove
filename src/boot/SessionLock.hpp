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

namespace Boot {
    typedef uint8_t u8;
    typedef int32_t i32;
    typedef uint32_t u32;
    using std::string, std::string_view, std::filesystem::path, std::filesystem::current_path, std::filesystem::remove, std::span, std::vector, std::to_string;

    struct SessionLock {
        [[nodiscard]] SessionLock(const string_view fileName) noexcept {
            lockFilePath = (current_path() / fileName).string();
        #if CG_PLATFORM_WINDOWS
        {
            lockFile = CreateFileA(lockFilePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (lockFile != INVALID_HANDLE_VALUE) {
                bool locked = LockFile(lockFile, 0, 0, 1, 0);
                if (!locked) goto invoke;
            }
            else goto invoke;
        }
        #else
        {
            i32 lockFile = open(lockFilePath.c_str(), O_CREAT | O_RDWR, 0666);
            if (lockFile >= 0) {
                bool locked = lockf(lockFile, F_TLOCK, 0) == 0;
                if (!locked) goto invoke;
            }
            else goto invoke;
        }
        #endif
        {
            const u32 pid = Util::OS::getProcessId();
            const span<const u8> pidBytes(reinterpret_cast<const u8*>(&pid), sizeof(pid));
            static_cast<void>(Util::OS::writeFile(lockFilePath, pidBytes, false));
        }
            return;
        invoke: {
            vector<u8> fileData;
            if (Util::OS::readFile(lockFilePath, fileData, false) && fileData.size() >= sizeof(u32)) {
                u32 pid;
                memcpy(&pid, fileData.data(), sizeof(u32));
            #if CG_PLATFORM_WINDOWS
                struct EnumData {
                    u32 pid;
                    HWND hwnd;
                };
                EnumData data{pid, nullptr};
                EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
                    auto& ed = *reinterpret_cast<EnumData*>(lParam);
                    DWORD windowPid = 0;
                    GetWindowThreadProcessId(hwnd, &windowPid);
                    if (windowPid == ed.pid && IsWindowVisible(hwnd)) {
                        ed.hwnd = hwnd;
                        return FALSE;
                    }
                    return TRUE;
                }, reinterpret_cast<LPARAM>(&data));
                if (data.hwnd) {
                    if (IsIconic(data.hwnd)) ShowWindow(data.hwnd, SW_RESTORE);
                    SetForegroundWindow(data.hwnd);
                }
            #elif CG_PLATFORM_LINUX
                const string cmd = string("wmctrl -l -p | awk '$3 == ") + to_string(pid) + " {print $1; exit}'";
                static_cast<void>(system(cmd.c_str()));
            #endif
            }
            Fatal::exit(Fatal::BOOT_MULTIPLE_INSTANCES);
        }
        }

        SessionLock(const SessionLock&) = delete;
        SessionLock(SessionLock&&) = delete;
        SessionLock& operator=(const SessionLock&) = delete;
        SessionLock& operator=(SessionLock&&) = delete;

        ~SessionLock() {
        #if CG_PLATFORM_WINDOWS
            UnlockFile(lockFile, 0, 0, 1, 0);
            CloseHandle(lockFile);
        #else
            lockf(lockFile, F_ULOCK, 0);
            close(lockFile);
        #endif
            remove(lockFilePath);
        }

    private:
        string lockFilePath;
    #if CG_PLATFORM_WINDOWS
        HANDLE lockFile;
    #else
        i32 lockFile;
    #endif
    };
}