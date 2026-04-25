#pragma once
#include <functional>
#include <vector>

#include "../debug/Logger.hpp"
#include "../util/SlotTable.hpp"
#include "InputKind.hpp"
#include "types.hpp"

namespace InputHandler {
    typedef uint64_t u64;
    using std::function, std::vector, Util::SlotTable, Util::GenerationalHandle;

    MAKE_DISTINCT_HANDLE(ActionHandle)

    template <typename ActionwiseInfo, typename EventwiseInfo>
    struct Action;

    template <typename ActionwiseInfo, typename EventwiseInfo>
    using ActionCallback = function<void(const SlotTable<Action<ActionwiseInfo, EventwiseInfo>, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo& eventwiseInfo, EventControlFlags& flags)>;

    template <typename ActionwiseInfo, typename EventwiseInfo>
    struct Action {
        ActionID actionId;
        ActionPriority priority;
        ActionCallback<ActionwiseInfo, EventwiseInfo> callback;
        ActionwiseInfo actionwiseInfo;

        [[nodiscard]] Action() noexcept = default;
        [[nodiscard]] Action(ActionID actionId, ActionPriority priority, ActionCallback<ActionwiseInfo, EventwiseInfo> callback, ActionwiseInfo actionwiseInfo) noexcept : actionId(actionId), priority(priority), callback(callback), actionwiseInfo(actionwiseInfo) {}
        [[nodiscard]] Action(const Action<ActionwiseInfo, EventwiseInfo>&) noexcept = default;
        [[nodiscard]] Action(Action<ActionwiseInfo, EventwiseInfo>&&) noexcept = default;
        Action<ActionwiseInfo, EventwiseInfo>& operator=(const Action<ActionwiseInfo, EventwiseInfo>&) noexcept = default;
        Action<ActionwiseInfo, EventwiseInfo>& operator=(Action<ActionwiseInfo, EventwiseInfo>&&) noexcept = default;

        friend Logger::Logger& operator<<(Logger::Logger& os, const Action<ActionwiseInfo, EventwiseInfo>& data) noexcept {
            os << "Action #" << data.actionId << " (priority " << data.priority << ", cb 0x" << reinterpret_cast<const u64*>(&data.callback) << ")";
            return os;
        }
    };

    inline constexpr ActionID INVALID_ACTION_ID = 0;

    struct ActionLocation {
        InputKind kind;
        ActionHandle actionHandle;
    };
}