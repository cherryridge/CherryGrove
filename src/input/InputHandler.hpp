#pragma once
#include <atomic>
#include <SDL3/SDL.h>

#include "Action.hpp"
#include "types.hpp"

namespace InputHandler {
    typedef uint64_t u64;
    using std::atomic;

    //todo: Eliminate these.
    extern atomic<bool> sendToImGui, sendToSimulation;

    inline constexpr u64 MAXIMUM_INPUT_EVENTS_PER_FRAME = 64;

    namespace internal {
        //This function is now single-threaded. There is really no point for it to be atomic provided that separate input kinds cannot add inputs concurrently within itself, and coordinating mods to add *different kinds* of inputs concurrently will probably bring more overhead.
        [[nodiscard]] ActionID getNextId() noexcept;
        void unregisterId(ActionID id) noexcept;
        void registerId(ActionID id, ActionLocation al) noexcept;

        inline constexpr const char* NULLPTR_ERROR = "[InputHandler] Unexpected nullptr encountered!";
        #define ASSERT_NOT_NULLPTR(ptr, ret)                       \
        if ((ptr) == nullptr) {                                    \
            lerr << InputHandler::internal::NULLPTR_ERROR << endl; \
            return ret;                                            \
        }

        #define ASSERT_CAN_DELETE(id, ret)                                                                       \
        if (!InputHandler::getCanDelete()) {                                                                     \
            lerr << "[InputHandler] Deletion attempted while deletion is not allowed. ActionID: " << id << endl; \
            return ret;                                                                                          \
        }
    }

    [[nodiscard]] bool getLocation(ActionID id, ActionLocation& result, InputKind expectedKind) noexcept;

    void init() noexcept;
    void shutdown() noexcept;

    [[nodiscard]] bool getCanDelete() noexcept;
    void setCanDelete(bool canDelete) noexcept;

    [[nodiscard]] bool getPointerLocked() noexcept;
    void setPointerLocked(bool locked) noexcept;

    //threaded: Main thread
    void submitEvent(const SDL_Event& event) noexcept;

    //threaded: Simulation thread
    void processTrigger() noexcept;
    void processPersist() noexcept;
}