#pragma once
#include <atomic>
#include <vector>
#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include "../../debug/loggers.hpp"
#include "../../util/SlotTable.hpp"
#include "../actionIds.hpp"
#include "../canDelete.hpp"
#include "../types.hpp"
#include "../utils.hpp"
#include "MMAction.hpp"

namespace InputHandler::MouseMove {
    using std::atomic, std::memory_order_acquire, std::memory_order_release, std::vector, glm::vec2, Util::SlotTable, InputHandler::internal::ActionLocation, InputHandler::internal::getLocation, InputHandler::internal::getNextId, InputHandler::internal::registerId, InputHandler::internal::unregisterId, InputHandler::utils::process, InputHandler::utils::insertSort, InputHandler::utils::remove;

    namespace detail {
        //This struct forces MSVC to use 8-byte alignment for vec2, which makes atomic<vec2> always lock-free across all platforms.
        struct alignas(8) IHateMSVC { vec2 pos; };
        inline atomic<IHateMSVC> lastMousePos;

        inline SlotTable<MouseMoveAction, ActionHandle> actionInfos;
        inline vector<ActionHandle> sortedTrigger, sortedPersist;
    }

//#region Actionwise API

    [[nodiscard]] inline ActionID add(MouseMoveActionCallback cb, ActionPriority priority, const ActionwiseInfo_MM& info) noexcept {
        const ActionID id = getNextId();
        const ActionHandle handle = detail::actionInfos.emplace(id, priority, cb, info);
        if (info.allowedKinds.get(SubKind::Trigger)) insertSort(detail::sortedTrigger, detail::actionInfos, handle);
        if (info.allowedKinds.get(SubKind::Persist)) insertSort(detail::sortedPersist, detail::actionInfos, handle);
        if (info.allowedKinds.none()) [[unlikely]] lerr << "[InputHandler::MouseMove] Action " << id << " is not listening to any SubKind. This action will never be triggered." << nlaf;
        registerId(id, {InputKind::MouseMove, handle});
        return id;
    }

    [[nodiscard]] inline bool remove(ActionID id) noexcept {
        ASSERT_CAN_DELETE(id, false)
        ActionLocation location;
        if (!getLocation(id, location, InputKind::MouseMove)) return false;

        const auto* actionInfo = detail::actionInfos.get(location.actionHandle);
        ASSERT_NOT_NULLPTR(actionInfo, return false;)

        if (actionInfo->actionwiseInfo.allowedKinds.get(SubKind::Trigger)) remove(detail::sortedTrigger, detail::actionInfos, id);
        if (actionInfo->actionwiseInfo.allowedKinds.get(SubKind::Persist)) remove(detail::sortedPersist, detail::actionInfos, id);

        static_cast<void>(detail::actionInfos.destroy(location.actionHandle));

        unregisterId(id);
        return true;
    }

    [[nodiscard]] inline bool get(ActionHandle handle, MouseMoveAction& result) noexcept {
        const MouseMoveAction* actionInfo = detail::actionInfos.get(handle);
        ASSERT_NOT_NULLPTR(actionInfo, return false;)
        result = *actionInfo;
        return true;
    }

    [[nodiscard]] inline bool get(ActionID id, MouseMoveAction& result) noexcept {
        ActionLocation location;
        if (!getLocation(id, location, InputKind::MouseMove)) return false;
        return get(location.actionHandle, result);
    }

//#endregion

//#region Event Processing

    inline void processTrigger(const SDL_Event& event) noexcept {
        if (event.type != SDL_EVENT_MOUSE_MOTION) {
            lerr << "[InputHandler::MouseMove] Unexpected event type: " << event.type << nlaf;
            return;
        }
        const EventwiseInfo_MM eventwiseInfo{
            .currentX = event.motion.x,
            .currentY = event.motion.y,
            .deltaX = event.motion.xrel,
            .deltaY = event.motion.yrel
        };
        detail::lastMousePos.store({{event.motion.x, event.motion.y}}, memory_order_release);
        process(detail::actionInfos, detail::sortedTrigger, eventwiseInfo);
    }

    inline void processPersist() noexcept {
        const auto pos = detail::lastMousePos.load(memory_order_acquire).pos;
        const EventwiseInfo_MM eventwiseInfo{
            .currentX = pos.x,
            .currentY = pos.y,
            .deltaX = 0,
            .deltaY = 0
        };
        process(detail::actionInfos, detail::sortedPersist, eventwiseInfo);
    }

//#endregion

//#region Misc

    [[nodiscard]] inline vec2 getMousePos() noexcept { return detail::lastMousePos.load(memory_order_acquire).pos; }

//#endregion
}