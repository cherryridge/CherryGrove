#pragma once
#include "../../util/os/platform.hpp" // IWYU pragma: keep
#if CG_PLATFORM_MACOS

#include <string>
#include <string_view>

#include "../../window.hpp"
#include "util.hpp"

namespace Boot::Focus {
    using std::string, std::string_view;

    namespace detail {
        inline sem_t* semaphore{SEM_FAILED};
        inline string semaphoreName;

        [[nodiscard]] inline string getEventName(const string_view lockFilePath) noexcept { return "/cgf." + hashPath(lockFilePath); }

        inline void raiseCurrentWindow() noexcept {
            auto* mainWindow = Window::getMainWindow();
            if (mainWindow == nullptr) return;
            SDL_SetHint(SDL_HINT_WINDOW_ACTIVATE_WHEN_RAISED, "1");
            SDL_SetHint(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, "1");
            SDL_ShowWindow(mainWindow);
            SDL_RestoreWindow(mainWindow);
            SDL_RaiseWindow(mainWindow);

            id nsWindow = reinterpret_cast<id>(Window::getPlatformHandle(mainWindow));
            if (nsWindow == nullptr) return;
            Class nsApplicationClass = reinterpret_cast<Class>(objc_getClass("NSApplication"));
            if (nsApplicationClass != nullptr) {
                id sharedApplication = reinterpret_cast<id (*)(id, SEL)>(objc_msgSend)(nsApplicationClass, sel_registerName("sharedApplication"));
                if (sharedApplication != nullptr) {
                    reinterpret_cast<void (*)(id, SEL, BOOL)>(objc_msgSend)(sharedApplication, sel_registerName("activateIgnoringOtherApps:"), YES);
                }
            }
            if (reinterpret_cast<BOOL (*)(id, SEL)>(objc_msgSend)(nsWindow, sel_registerName("isMiniaturized"))) reinterpret_cast<void (*)(id, SEL, id)>(objc_msgSend)(nsWindow, sel_registerName("deminiaturize:"), nullptr);
            reinterpret_cast<void (*)(id, SEL, id)>(objc_msgSend)(nsWindow, sel_registerName("makeKeyAndOrderFront:"), nullptr);
            reinterpret_cast<void (*)(id, SEL)>(objc_msgSend)(nsWindow, sel_registerName("orderFrontRegardless"));
        }
    }

    inline void connect(const string_view lockFilePath) noexcept {
        if (detail::semaphore != SEM_FAILED) return;
        detail::semaphoreName = detail::getEventName(lockFilePath);
        detail::semaphore = sem_open(detail::semaphoreName.c_str(), O_CREAT, 0600, 0);
        if (detail::semaphore == SEM_FAILED) {
            detail::semaphoreName.clear();
            return;
        }
        while (sem_trywait(detail::semaphore) == 0) {}
    }

    inline void disconnect() noexcept {
        if (detail::semaphore == SEM_FAILED) return;
        static_cast<void>(sem_close(detail::semaphore));
        if (!detail::semaphoreName.empty()) static_cast<void>(sem_unlink(detail::semaphoreName.c_str()));
        detail::semaphore = SEM_FAILED;
        detail::semaphoreName.clear();
    }

    inline void tryReceive() noexcept {
        if (detail::semaphore == SEM_FAILED) return;
        if (sem_trywait(detail::semaphore) != 0) return;
        detail::raiseCurrentWindow();
        while (sem_trywait(detail::semaphore) == 0) {}
    }

    inline void sendMessage(const string_view lockFilePath) noexcept {
        sem_t* semaphore = sem_open(detail::getEventName(lockFilePath).c_str(), 0);
        if (semaphore == SEM_FAILED) return;
        static_cast<void>(sem_post(semaphore));
        static_cast<void>(sem_close(semaphore));
    }
}
#endif