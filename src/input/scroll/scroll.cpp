#include <vector>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"
#include "../../util/SlotTable.hpp"
#include "../InputHandler.hpp"
#include "../utils.hpp"
#include "SCAction.hpp"

#include "scroll.hpp"

namespace InputHandler::Scroll {
    typedef uint8_t u8;
    typedef uint32_t u32;
    using std::vector, Util::SlotTable;

    static SlotTable<ScrollAction, ActionHandle> actionInfos;
    static vector<ActionHandle> sortedBoth, sortedHorizontal, sortedVertical;

    [[nodiscard]] ActionID add(ScrollActionCallback cb, ActionPriority priority, const ActionwiseInfo_SC& info) noexcept {
        const ActionID id = InputHandler::internal::getNextId();
        const ActionHandle handle = actionInfos.emplace(id, priority, cb, info);
        const auto* newAction = actionInfos.get(handle);
        if (info.directions.all()) InputHandler::insertSort(sortedBoth, actionInfos, handle);
        else if (info.directions.get(Direction::Horizontal)) InputHandler::insertSort(sortedHorizontal, actionInfos, handle);
        else if (info.directions.get(Direction::Vertical)) InputHandler::insertSort(sortedVertical, actionInfos, handle);
        else [[unlikely]] {
        #if CG_DEBUG
            lerr << "[InputHandler::Scroll] Action " << id << " is not listening to any direction. This action will never be triggered." << endl;
        #endif
        }
        InputHandler::insertSort(sortedBoth, actionInfos, handle);
        InputHandler::internal::registerId(id, {InputKind::Scroll, handle});
        return id;
    }

    [[nodiscard]] bool remove(ActionID id) noexcept {
    #if CG_DEBUG
        ASSERT_CAN_DELETE(id, false)
    #endif
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::Scroll)) return false;
        const auto* actionInfo = actionInfos.get(location.actionHandle);
        if (actionInfo == nullptr) return false;
        const auto directions = actionInfo->actionwiseInfo.directions;
        static_cast<void>(actionInfos.destroy(location.actionHandle));
        if (directions.all()) InputHandler::remove(sortedBoth, actionInfos, id);
        else if (directions.get(Direction::Horizontal)) InputHandler::remove(sortedHorizontal, actionInfos, id);
        else if (directions.get(Direction::Vertical)) InputHandler::remove(sortedVertical, actionInfos, id);
        InputHandler::internal::unregisterId(id);
        return true;
    }

    [[nodiscard]] bool get(ActionHandle handle, ScrollAction& result) noexcept {
        const ScrollAction* actionInfo = actionInfos.get(handle);
        if (actionInfo == nullptr) return false;
        result = *actionInfo;
        return true;
    }

    [[nodiscard]] bool get(ActionID id, ScrollAction& result) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::Scroll)) return false;
        return get(location.actionHandle, result);
    }

    void processTrigger(const SDL_Event& event) noexcept {
    #if CG_DEBUG
        if (event.type != SDL_EVENT_MOUSE_WHEEL) {
            lerr << "[InputHandler::Scroll] Unexpected event type: " << event.type << endl;
            return;
        }
    #endif
        const u8 inverted = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1;
        EventwiseInfo_SC eventwiseInfo {
            .scrollX = event.wheel.x * inverted,
            .scrollY = event.wheel.y * inverted,
            .mouseX = event.wheel.mouse_x,
            .mouseY = event.wheel.mouse_y,
            .discreteX = event.wheel.integer_x * inverted,
            .discreteY = event.wheel.integer_y * inverted,
            .inverted = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED
        };
        if (eventwiseInfo.scrollX != 0 || eventwiseInfo.scrollY != 0) InputHandler::process(actionInfos, sortedBoth, eventwiseInfo);
        else if (eventwiseInfo.scrollX != 0) InputHandler::process(actionInfos, sortedHorizontal, eventwiseInfo);
        else if (eventwiseInfo.scrollY != 0) InputHandler::process(actionInfos, sortedVertical, eventwiseInfo);
    }
}