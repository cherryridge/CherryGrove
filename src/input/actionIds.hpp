#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

#include "../debug/loggers.hpp" // IWYU pragma: keep
#include "Action.hpp"
#include "InputKind.hpp"
#include "types.hpp"

namespace InputHandler::internal {
    using boost::unordered_flat_map;

    struct ActionLocation {
        InputKind kind;
        ActionHandle actionHandle;
    };

    namespace detail {
        inline unordered_flat_map<ActionID, ActionLocation> actionIdToKind;
        inline ActionID nextId{1};
    }

    //This function is now single-threaded. There is really no point for it to be atomic provided that separate input kinds cannot add inputs concurrently within itself, and coordinating mods to add *different kinds* of inputs concurrently will probably bring more overhead.
    [[nodiscard]] inline ActionID getNextId() noexcept {
        const auto id = detail::nextId;
        detail::nextId++;
        return id;
    }

    inline void registerId(ActionID id, ActionLocation al) noexcept { detail::actionIdToKind.emplace(id, al); }

    inline void unregisterId(ActionID id) noexcept { detail::actionIdToKind.erase(id); }

    [[nodiscard]] inline bool getLocation(ActionID id, ActionLocation& result, InputKind expectedKind) noexcept {
        const auto it = detail::actionIdToKind.find(id);
        if (it == detail::actionIdToKind.end()) return false;
        else {
            result = it->second;
            return result.kind == expectedKind;
        }
    }

    inline constexpr const char* NULLPTR_ERROR = "[InputHandler] Unexpected nullptr encountered!";
    #define ASSERT_NOT_NULLPTR(ptr, doSomething) \
    if ((ptr) == nullptr) { \
        lerr << InputHandler::internal::NULLPTR_ERROR << nlaf; \
        doSomething \
    }
}