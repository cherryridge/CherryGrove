#pragma once
#include <vector>
#include <function2/function2.hpp>

#include "../debug/Logger.hpp"
#include "../util/SlotTable.hpp"
#include "types.hpp"

namespace InputHandler {
    using std::vector, fu2::function_view, Util::SlotTable, Util::GenerationalHandle;

    MAKE_DISTINCT_HANDLE(ActionHandle)

    template <typename ActionwiseInfo, typename EventwiseInfo>
    struct Action;

    template <typename ActionwiseInfo, typename EventwiseInfo>
    using ActionCallback = function_view<void(const SlotTable<Action<ActionwiseInfo, EventwiseInfo>, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo& eventwiseInfo, EventControlFlags& flags)>;

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
            os << "Action #" << data.actionId << " (priority " << data.priority << ", cb 0x" << reinterpret_cast<void*>(&data.callback) << ")";
            return os;
        }
    };

    inline constexpr ActionID INVALID_ACTION_ID = 0;
}