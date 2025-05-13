#include <mutex>
#include <map>
#include <GLFW/glfw3.h>

#include "../../CherryGrove.hpp"
#include "../ImGuiAdapter.hpp"
#include "../inputBase.hpp"
#include "mouseMove.hpp"

namespace InputHandler::MouseMove {
    typedef int32_t i32;
    using std::mutex, std::multimap, std::lock_guard;

    static multimap<EventPriority, MMEvent> mmRegistry;
    static mutex mmMutex;

    static MMEventInfo store;

    void init() {}

    void s_cursorPosCB(GLFWwindow* window, double xpos, double ypos) {
        if (CherryGrove::isCGAlive) {
            if (sendToImGui) imGui_cursorPosCB(window, xpos, ypos);
            store.originX = store.newX;
            store.originY = store.newY;
            store.newX = xpos;
            store.newY = ypos;
        }
    }

    void s_cursorEnterCB(GLFWwindow* window, i32 entered) {
        if (CherryGrove::isCGAlive) {
            if (sendToImGui) imGui_cursorEnterCB(window, entered);
            if (entered) {
                //todo
            }
        }
    }

    void addMouseMove(const InputEventInfo& info, MMCallback cb) {
        MMEvent event(info, cb);
        lock_guard lock(mmMutex);
        mmRegistry.emplace(info.priority, event);
    }

    bool removeMouseMove(MMCallback cb) {
        for (auto it = mmRegistry.begin(); it != mmRegistry.end(); it++) if (it->second.cb == cb) {
            lock_guard lock(mmMutex);
            mmRegistry.erase(it);
            return true;
        }
        return false;
    }

    void s_process() {
        for (const auto& [priority, event] : mmRegistry) {
            EventFlags flags = 0;
            event.cb(mmRegistry, priority, flags, store);
            if (flags & EVENTFLAGS_STOP_IMMEDIATELY) break;
        }
        store.originX = store.newX;
        store.originY = store.newY;
    }
}