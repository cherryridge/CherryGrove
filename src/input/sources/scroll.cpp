#include <glfw/glfw3.h>
#include <map>
#include <mutex>

#include "../../debug/debug.hpp"
#include "../../CherryGrove.hpp"

#include "../ImGuiAdapter.hpp"

#include "../inputBase.hpp"

#include "scroll.hpp"

namespace InputHandler::Scroll {
	typedef int32_t i32;
	using std::multimap, std::mutex, std::lock_guard;

	static multimap<EventPriority, ScrollEvent> scrollRegistry;
	static mutex scrollMutex;

	static ScrollEventInfo store;

	void init() {}

	void s_scrollCB(GLFWwindow* window, double xoffset, double yoffset) {
		if (CherryGrove::isCGAlive) {
			if (sendToImGui) imGui_scrollCB(window, xoffset, yoffset);
			store.xOffset = xoffset;
			store.yOffset = yoffset;
		}
	}

	void addScroll(const InputEventInfo& info, ScrollCallback cb) {
		ScrollEvent event(info, cb);
		lock_guard lock(scrollMutex);
		scrollRegistry.emplace(info.priority, event);
	}

	bool removeScroll(ScrollCallback cb) {
		for (auto it = scrollRegistry.begin(); it != scrollRegistry.end(); it++) if(it->second.cb == cb) {
			lock_guard lock(scrollMutex);
			scrollRegistry.erase(it);
			return true;
		}
		return false;
	}

	void s_process() {
		for (const auto& [priority, event] : scrollRegistry) {
			EventFlags flags = 0;
			event.cb(scrollRegistry, priority, flags, store);
			if (flags & EVENTFLAGS_STOP_IMMEDIATELY) break;
		}
	}
}