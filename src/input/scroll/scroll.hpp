#pragma once
#include <vector>
#include <SDL3/SDL.h>

#include "../../debug/loggers.hpp"
#include "../../util/SlotTable.hpp"
#include "../actionIds.hpp"
#include "../canDelete.hpp"
#include "../types.hpp"
#include "../utils.hpp"
#include "SCAction.hpp"

namespace InputHandler::Scroll {
    typedef uint8_t u8;
    typedef uint32_t u32;
    using std::vector, Util::SlotTable, InputHandler::internal::ActionLocation, InputHandler::internal::getLocation, InputHandler::internal::getNextId, InputHandler::internal::registerId, InputHandler::internal::unregisterId, InputHandler::utils::process, InputHandler::utils::insertSort, InputHandler::utils::remove;
    
//#region Actionwise API

    namespace detail {
        inline SlotTable<ScrollAction, ActionHandle> actionInfos;
        inline vector<ActionHandle> sortedBoth, sortedHorizontal, sortedVertical;
    }

    [[nodiscard]] inline ActionID add(ScrollActionCallback cb, ActionPriority priority, const ActionwiseInfo_SC& info) noexcept {
        const ActionID id = getNextId();
        const ActionHandle handle = detail::actionInfos.emplace(id, priority, cb, info);
        const auto* newAction = detail::actionInfos.get(handle);
        if (info.directions.all()) insertSort(detail::sortedBoth, detail::actionInfos, handle);
        else if (info.directions.get(Direction::Horizontal)) insertSort(detail::sortedHorizontal, detail::actionInfos, handle);
        else if (info.directions.get(Direction::Vertical)) insertSort(detail::sortedVertical, detail::actionInfos, handle);
        else [[unlikely]] lerr << "[InputHandler::Scroll] Action " << id << " is not listening to any direction. This action will never be triggered." << nlaf;
        insertSort(detail::sortedBoth, detail::actionInfos, handle);
        registerId(id, {InputKind::Scroll, handle});
        return id;
    }

    [[nodiscard]] inline bool remove(ActionID id) noexcept {
        ASSERT_CAN_DELETE(id, false)
        ActionLocation location;
        if (!getLocation(id, location, InputKind::Scroll)) return false;

        const auto* actionInfo = detail::actionInfos.get(location.actionHandle);
        ASSERT_NOT_NULLPTR(actionInfo, return false;)

        if (actionInfo->actionwiseInfo.directions.all()) remove(detail::sortedBoth, detail::actionInfos, id);
        else if (actionInfo->actionwiseInfo.directions.get(Direction::Horizontal)) remove(detail::sortedHorizontal, detail::actionInfos, id);
        else if (actionInfo->actionwiseInfo.directions.get(Direction::Vertical)) remove(detail::sortedVertical, detail::actionInfos, id);

        static_cast<void>(detail::actionInfos.destroy(location.actionHandle));

        unregisterId(id);
        return true;
    }

    [[nodiscard]] inline bool get(ActionHandle handle, ScrollAction& result) noexcept {
        const ScrollAction* actionInfo = detail::actionInfos.get(handle);
        ASSERT_NOT_NULLPTR(actionInfo, return false;)
        result = *actionInfo;
        return true;
    }

    [[nodiscard]] inline bool get(ActionID id, ScrollAction& result) noexcept {
        ActionLocation location;
        if (!getLocation(id, location, InputKind::Scroll)) return false;
        return get(location.actionHandle, result);
    }

//#endregion

//#region Event Processing

    inline void processTrigger(const SDL_Event& event) noexcept {
        if (event.type != SDL_EVENT_MOUSE_WHEEL) {
            lerr << "[InputHandler::Scroll] Unexpected event type: " << event.type << nlaf;
            return;
        }
        const u8 inverted = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1;
        const EventwiseInfo_SC eventwiseInfo {
            .scrollX = event.wheel.x * inverted,
            .scrollY = event.wheel.y * inverted,
            .mouseX = event.wheel.mouse_x,
            .mouseY = event.wheel.mouse_y,
            .discreteX = event.wheel.integer_x * inverted,
            .discreteY = event.wheel.integer_y * inverted,
            .inverted = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED
        };
        if (eventwiseInfo.scrollX != 0 || eventwiseInfo.scrollY != 0) process(detail::actionInfos, detail::sortedBoth, eventwiseInfo);
        else if (eventwiseInfo.scrollX != 0) process(detail::actionInfos, detail::sortedHorizontal, eventwiseInfo);
        else if (eventwiseInfo.scrollY != 0) process(detail::actionInfos, detail::sortedVertical, eventwiseInfo);
    }

//#endregion
}