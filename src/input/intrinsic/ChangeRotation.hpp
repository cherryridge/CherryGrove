#pragma once
#include <map>
#include <atomic>

#include "../InputHandler.hpp"

namespace IntrinsicInput {
	using namespace InputHandler;
	extern std::atomic<double> cameraSensitivity;

	void changeRotationCB(const std::multimap<EventPriority, MouseMove::MMEvent>& events, EventPriority priority, EventFlags flags, const MouseMove::MMEventInfo& info);
}