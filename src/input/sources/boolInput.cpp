#include <unordered_map>
#include <map>
#include <mutex>

#include "../../debug/debug.hpp"
#include "../../CherryGrove.hpp"

#include "../ImGuiAdapter.hpp"

#include "../inputBase.hpp"

#include "boolInput.hpp"

namespace InputHandler::BoolInput {
	typedef int32_t i32;
	using std::unordered_map, std::multimap, std::mutex, std::unique_lock, std::lock_guard;

	//This is not a continuous map. See `BoolInputID`.
	static unordered_map<BoolInputID, BIStatus> biStatus;
	static mutex biStatusMutex;

	//Explanation for BIEvent: see boolInput.hpp.
	static unordered_map<BoolInputID, multimap<EventPriority, BIEvent>> biStartRegistry;
	static mutex biStartMutex;
	static unordered_map<BoolInputID, multimap<EventPriority, BIEvent>> biPersistRegistry;
	static mutex biPersistMutex;
	static unordered_map<BoolInputID, multimap<EventPriority, BIEvent>> biEndRegistry;
	static mutex biEndMutex;

	void init() {
		for (auto& bi : boolInputDesc) {
			biStatus.emplace(bi.inputCode, BIInactive);
			multimap<EventPriority, BIEvent> start, persist, end;
			biStartRegistry.emplace(bi.inputCode, move(start));
			biPersistRegistry.emplace(bi.inputCode, move(persist));
			biEndRegistry.emplace(bi.inputCode, move(end));
		}
	}

	void s_keyCB(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
		if (CherryGrove::isCGAlive) {
			//lout << "?" << endl;
			if (sendToImGui) imGui_keyCB(window, key, scancode, action, mods);
			switch (action) {
				case GLFW_PRESS: {
					lock_guard lock(biStatusMutex);
					auto p = biStatus.find(key);
					if (p != biStatus.end()) p->second = BIPress;
					//lout << "press" << endl;
					break;
				}
				case GLFW_REPEAT: break;
				case GLFW_RELEASE: {
					lock_guard lock(biStatusMutex);
					auto p = biStatus.find(key);
					if (p != biStatus.end()) p->second = BIRelease;
					//lout << "rel" << endl;
					break;
				}
			}
		}
	}

	void s_mouseButtonCB(GLFWwindow* window, i32 button, i32 action, i32 mods) {
		if (CherryGrove::isCGAlive) {
			if (sendToImGui) imGui_mouseButtonCB(window, button, action, mods);
			switch (action) {
				case GLFW_PRESS: {
					lock_guard lock(biStatusMutex);
					auto p = biStatus.find(button);
					if (p != biStatus.end()) p->second = BIPress;
					break;
				}
				case GLFW_REPEAT: break;
				case GLFW_RELEASE: {
					lock_guard lock(biStatusMutex);
					auto p = biStatus.find(button);
					if (p != biStatus.end()) p->second = BIRelease;
					break;
				}
			}
		}
	}

	constexpr const char* findBoolInputName(BoolInputID inputCode) {
		auto it = std::lower_bound(boolInputDesc.begin(), boolInputDesc.end(), inputCode, [](const BISource& descriptor, BoolInputID key) {
			return descriptor.inputCode < key;
		});
		if (it != boolInputDesc.end()) return it->name;
		else return nullptr;
	}

	void addBoolInput(BIEventType type, const InputEventInfo& info, BICallback cb, BoolInputID defaultBinding) {
		auto& eventRegistry = type == BIEventStart ? biStartRegistry : type == BIEventPersist ? biPersistRegistry : biEndRegistry;
		auto p = eventRegistry.find(defaultBinding);
		if (biStatus.find(defaultBinding) != biStatus.end()) {
			lock_guard eventRegistryLock(type == BIEventStart ? biStartMutex : type == BIEventPersist ? biPersistMutex : biEndMutex);
			BIEvent event(info, cb, defaultBinding);
			(p->second).emplace(info.priority, event);
		}
		else lerr << "[InputHandler] Passed invalid bool input ID: " << defaultBinding << endl;
	}

	bool removeBoolInput(BIEventType type, BICallback cb) {
		for (auto& [iid, map] : (type == BIEventStart ? biStartRegistry : type == BIEventPersist ? biPersistRegistry : biEndRegistry)) {
			for (auto it = map.begin(); it != map.end(); it++) if (it->second.cb == cb) {
				lock_guard eventRegistryLock(type == BIEventStart ? biStartMutex : type == BIEventPersist ? biPersistMutex : biEndMutex);
				map.erase(it);
				return true;
			}
		}
		return false;
	}

	void changeBinding() {

	}

	void resetBinding() {

	}

	void s_process() {
		//lout << "Processing Bool Input" << endl;
		for (const auto& [id, status] : biStatus) switch (status) {
			case BIPress: {
				{ //Change the status to `BIRepeat` instantly.
					lock_guard lock(biStatusMutex);
					biStatus[id] = BIRepeat;
				}
				auto p = biStartRegistry.find(id);
				if (p != biStartRegistry.end()) for (const auto& [priority, event] : p->second) {
					//lout << "BI Press " << event.info.name << endl;
					EventFlags flags = 0;
					event.cb(p->second, priority, flags, id);
					if (flags & EVENTFLAGS_STOP_IMMEDIATELY) break;
				}
				break;
			}
			case BIRepeat: {
				auto p = biPersistRegistry.find(id);
				if (p != biPersistRegistry.end()) for (const auto& [priority, event] : p->second) {
					//lout << "BI Repeat " << event.info.name << endl;
					EventFlags flags = 0;
					event.cb(p->second, priority, flags, id);
					if (flags & EVENTFLAGS_STOP_IMMEDIATELY) break;
				}
				break;
			}
			case BIRelease: {
				{ //Change the status to `BIInactive` instantly.
					lock_guard lock(biStatusMutex);
					biStatus[id] = BIInactive;
				}
				auto p = biEndRegistry.find(id);
				if (p != biEndRegistry.end()) for (const auto& [priority, event] : p->second) {
					//lout << "BI Release " << event.info.name << endl;
					EventFlags flags = 0;
					event.cb(p->second, priority, flags, id);
					if (flags & EVENTFLAGS_STOP_IMMEDIATELY) break;
				}
				break;
			}
			case BIInactive: default: break;
		}
	}
}