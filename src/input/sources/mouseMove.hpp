#pragma once
#include <glfw/glfw3.h>
#include <map>

#include "../inputBase.hpp"

namespace InputHandler::MouseMove {
	typedef int32_t i32;

	struct MMEventInfo {
		double originX;
		double originY;
		double newX;
		double newY;

		MMEventInfo() : originX(0.0), originY(0.0), newX(0.0), newY(0.0) {}
	};

	struct MMEvent;

	using MMCallback = void(*)(const std::multimap<EventPriority, MMEvent>& events, EventPriority priority, EventFlags flags, const MMEventInfo& info);

	struct MMEvent {
		InputEventInfo info;
		MMCallback cb;

		MMEvent(const InputEventInfo& info, MMCallback cb) : info(info), cb(cb) {}
	};

	void init();

	void s_cursorPosCB(GLFWwindow* window, double xpos, double ypos);
	void s_cursorEnterCB(GLFWwindow* window, i32 entered);

	void addMouseMove(const InputEventInfo& info, MMCallback cb);
	bool removeMouseMove(MMCallback cb);

	void s_process();
}