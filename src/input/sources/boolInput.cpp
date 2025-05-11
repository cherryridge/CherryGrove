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
			biStatus.emplace(bi.inputCode, BIStatus::Inactive);
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
					if (p != biStatus.end()) p->second = BIStatus::Press;
					//lout << "press" << endl;
					break;
				}
				case GLFW_REPEAT: break;
				case GLFW_RELEASE: {
					lock_guard lock(biStatusMutex);
					auto p = biStatus.find(key);
					if (p != biStatus.end()) p->second = BIStatus::Release;
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
					if (p != biStatus.end()) p->second = BIStatus::Press;
					break;
				}
				case GLFW_REPEAT: break;
				case GLFW_RELEASE: {
					lock_guard lock(biStatusMutex);
					auto p = biStatus.find(button);
					if (p != biStatus.end()) p->second = BIStatus::Release;
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

	void addBoolInput(BIEType type, const InputEventInfo& info, BICallback cb, BoolInputID defaultBinding) {
		auto& eventRegistry = type == BIEType::Start ? biStartRegistry : type == BIEType::Persist ? biPersistRegistry : biEndRegistry;
		auto p = eventRegistry.find(defaultBinding);
		if (biStatus.find(defaultBinding) != biStatus.end()) {
			lock_guard eventRegistryLock(type == BIEType::Start ? biStartMutex : type == BIEType::Persist ? biPersistMutex : biEndMutex);
			BIEvent event(info, cb, defaultBinding);
			(p->second).emplace(info.priority, event);
		}
		else lerr << "[InputHandler] Passed invalid bool input ID: " << defaultBinding << endl;
	}

	bool removeBoolInput(BIEType type, BICallback cb) {
		for (auto& [iid, map] : (type == BIEType::Start ? biStartRegistry : type == BIEType::Persist ? biPersistRegistry : biEndRegistry)) {
			for (auto it = map.begin(); it != map.end(); it++) if (it->second.cb == cb) {
				lock_guard eventRegistryLock(type == BIEType::Start ? biStartMutex : type == BIEType::Persist ? biPersistMutex : biEndMutex);
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
			case BIStatus::Press: {
				{ //Change the status to `Repeat` instantly.
					lock_guard lock(biStatusMutex);
					biStatus[id] = BIStatus::Repeat;
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
			case BIStatus::Repeat: {
				auto p = biPersistRegistry.find(id);
				if (p != biPersistRegistry.end()) for (const auto& [priority, event] : p->second) {
					//lout << "BI Repeat " << event.info.name << endl;
					EventFlags flags = 0;
					event.cb(p->second, priority, flags, id);
					if (flags & EVENTFLAGS_STOP_IMMEDIATELY) break;
				}
				break;
			}
			case BIStatus::Release: {
				{ //Change the status to `BIInactive` instantly.
					lock_guard lock(biStatusMutex);
					biStatus[id] = BIStatus::Inactive;
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
			//case BIStatus::Inactive:
			default: break;
		}
	}
}