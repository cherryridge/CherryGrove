#include <atomic>
#include <vector>
#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"
#include "../../util/SlotTable.hpp"
#include "../InputHandler.hpp"
#include "../utils.hpp"
#include "MMAction.hpp"

#include "mouseMove.hpp"

namespace InputHandler::MouseMove {
    using std::atomic, std::memory_order_acquire, std::memory_order_release, std::vector, Util::SlotTable;

    //This struct forces MSVC to use 8-byte alignment for vec2, which makes atomic<vec2> always lock-free across all platforms.
    struct alignas(8) IHateMSVC { vec2 pos; };
    static atomic<IHateMSVC> lastMousePos;

    static SlotTable<MouseMoveAction, ActionHandle> actionInfos;
    static vector<ActionHandle> sortedTrigger, sortedPersist;

    [[nodiscard]] ActionID add(MouseMoveActionCallback cb, ActionPriority priority, const ActionwiseInfo_MM& info) noexcept {
        const ActionID id = InputHandler::internal::getNextId();
        const ActionHandle handle = actionInfos.emplace(id, priority, cb, info);
        if (info.allowedKinds.get(SubKind::Trigger)) InputHandler::insertSort(sortedTrigger, actionInfos, handle);
        if (info.allowedKinds.get(SubKind::Persist)) InputHandler::insertSort(sortedPersist, actionInfos, handle);
    #if CG_DEBUG
        if (info.allowedKinds.none()) [[unlikely]] lerr << "[InputHandler::MouseMove] Action " << id << " is not listening to any SubKind. This action will never be triggered." << endl;
    #endif
        InputHandler::internal::registerId(id, {InputKind::MouseMove, handle});
        return id;
    }

    [[nodiscard]] bool remove(ActionID id) noexcept {
    #if CG_DEBUG
        ASSERT_CAN_DELETE(id, false)
    #endif
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::MouseMove)) return false;
        const auto* actionInfo = actionInfos.get(location.actionHandle);
        if (actionInfo == nullptr) return false;
        const auto allowedKinds = actionInfo->actionwiseInfo.allowedKinds;
        static_cast<void>(actionInfos.destroy(location.actionHandle));
        if (allowedKinds.get(SubKind::Trigger)) InputHandler::remove(sortedTrigger, actionInfos, id);
        if (allowedKinds.get(SubKind::Persist)) InputHandler::remove(sortedPersist, actionInfos, id);
        InputHandler::internal::unregisterId(id);
        return true;
    }

    [[nodiscard]] bool get(ActionHandle handle, MouseMoveAction& result) noexcept {
        const MouseMoveAction* actionInfo = actionInfos.get(handle);
        if (actionInfo == nullptr) return false;
        result = *actionInfo;
        return true;
    }

    [[nodiscard]] bool get(ActionID id, MouseMoveAction& result) noexcept {
        ActionLocation location;
        if (!InputHandler::getLocation(id, location, InputKind::MouseMove)) return false;
        return get(location.actionHandle, result);
    }

    void processTrigger(const SDL_Event& event) noexcept {
    #if CG_DEBUG
        if (event.type != SDL_EVENT_MOUSE_MOTION) {
            lerr << "[InputHandler::MouseMove] Unexpected event type: " << event.type << endl;
            return;
        }
    #endif
        EventwiseInfo_MM eventwiseInfo{
            .currentX = event.motion.x,
            .currentY = event.motion.y,
            .deltaX = event.motion.xrel,
            .deltaY = event.motion.yrel
        };
        lastMousePos.store({{event.motion.x, event.motion.y}}, memory_order_release);
        InputHandler::process(actionInfos, sortedTrigger, eventwiseInfo);
    }

    void processPersist() noexcept {
        const auto pos = lastMousePos.load(memory_order_acquire).pos;
        EventwiseInfo_MM eventwiseInfo{
            .currentX = pos.x,
            .currentY = pos.y,
            .deltaX = 0,
            .deltaY = 0
        };
        InputHandler::process(actionInfos, sortedPersist, eventwiseInfo);
    }

    [[nodiscard]] vec2 getMousePos() noexcept { return lastMousePos.load(memory_order_acquire).pos; }
}