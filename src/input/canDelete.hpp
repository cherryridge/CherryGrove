#pragma once
#include <atomic>

namespace InputHandler {
    using std::atomic, std::memory_order_acquire, std::memory_order_release;

    namespace detail {
        inline atomic<bool> canDelete{true};
    }

    [[nodiscard]] inline bool getCanDelete() noexcept { return detail::canDelete.load(memory_order_acquire); }

    inline void setCanDelete(bool canDelete) noexcept { detail::canDelete.store(canDelete, memory_order_release); }

    #define ASSERT_CAN_DELETE(id, ret) \
    if (!InputHandler::getCanDelete()) { \
        lerr << "[InputHandler] Deletion attempted while deletion is not allowed. ActionID: " << id << nlaf; \
        return ret; \
    }
}