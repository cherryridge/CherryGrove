#pragma once
#include <GLFW/glfw3.h>
#include <map>

#include "../inputBase.hpp"

namespace InputHandler::Scroll {
	struct ScrollEventInfo {
		double xOffset;
		double yOffset;

		ScrollEventInfo() : xOffset(0.0), yOffset(0.0) {}
	};

	struct ScrollEvent;

	using ScrollCallback = void(*)(const std::multimap<EventPriority, ScrollEvent>& events, EventPriority priority, EventFlags flags, const ScrollEventInfo& info);

	struct ScrollEvent {
		InputEventInfo info;
		ScrollCallback cb;

		ScrollEvent(InputEventInfo info, ScrollCallback cb) : info(info), cb(cb) {}
	};

	void init();

	void s_scrollCB(GLFWwindow* window, double xoffset, double yoffset);

	void addScroll(const InputEventInfo& info, ScrollCallback cb);
	bool removeScroll(ScrollCallback cb);

	void s_process();
}