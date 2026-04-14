#pragma once
#include "../../util/os/platform.hpp" // IWYU pragma: keep
#if CG_PLATFORM_WINDOWS

#include <string>
#include <string_view>

#include "../../window.hpp"
#include "util.hpp"

namespace Boot::Focus {
    using std::string, std::string_view;
    
    namespace detail {
        inline HANDLE eventHandle{nullptr};

        [[nodiscard]] inline string getEventName(const string_view lockFilePath) noexcept { return "Local\\cgf." + hashPath(lockFilePath); }

        inline void raiseCurrentWindow() noexcept {
            auto* mainWindow = Window::getMainWindow();
            if (mainWindow == nullptr) return;
            HWND hwnd = reinterpret_cast<HWND>(Window::getPlatformHandle(mainWindow));
            if (hwnd == nullptr) return;
            ShowWindow(hwnd, IsIconic(hwnd) ? SW_RESTORE : SW_SHOW);
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
            BringWindowToTop(hwnd);
            SetForegroundWindow(hwnd);
            SetActiveWindow(hwnd);
            SetFocus(hwnd);
        }
    }

    inline void connect(const string_view lockFilePath) noexcept {
        if (detail::eventHandle != nullptr) return;
        detail::eventHandle = CreateEventA(nullptr, FALSE, FALSE, detail::getEventName(lockFilePath).c_str());
    }

    inline void disconnect() noexcept {
        if (detail::eventHandle == nullptr) return;
        CloseHandle(detail::eventHandle);
        detail::eventHandle = nullptr;
    }

    inline void tryReceive() noexcept {
        if (detail::eventHandle == nullptr) return;
        if (WaitForSingleObject(detail::eventHandle, 0) == WAIT_OBJECT_0) detail::raiseCurrentWindow();
    }

    inline void sendMessage(const string_view lockFilePath) noexcept {
        HANDLE focusEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, detail::getEventName(lockFilePath).c_str());
        if (focusEvent == nullptr) return;
        SetEvent(focusEvent);
        CloseHandle(focusEvent);
    }
}
#endif