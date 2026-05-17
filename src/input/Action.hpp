#pragma once
#include <cstdint>
#include <function2/function2.hpp>

#include "../debug/implLShiftFor.hpp"
#include "../util/SlotTable.hpp"
#include "types.hpp"

namespace InputHandler {
    using fu2::function_view, Util::SlotTable, Util::GenerationalHandle;

    MAKE_DISTINCT_HANDLE(ActionHandle)

    template <typename ActionwiseInfo, typename EventwiseInfo>
    struct Action;

    template <typename ActionwiseInfo, typename EventwiseInfo>
    using ActionCallback = function_view<void(const SlotTable<Action<ActionwiseInfo, EventwiseInfo>, ActionHandle>& actionInfos, ActionHandle handle, const EventwiseInfo& eventwiseInfo, EventControlFlags& flags) const>;

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

        using SpecializedAction = Action<ActionwiseInfo, EventwiseInfo>;

        IMPL_LSHIFT_FOR(SpecializedAction,
            output << "Action #" << data.actionId << " (priority " << data.priority << ", cb 0x" << reinterpret_cast<uintptr_t>(&data.callback) << ")";
        )
    };

    inline constexpr ActionID INVALID_ACTION_ID = 0;
}