#include <glfw/glfw3.h>
#include <atomic>
#include <vector>

#include "InputHandler.hpp"

namespace InputHandler {
	typedef uint32_t u32;

	using std::atomic, std::vector, std::pair, std::make_pair;

	atomic<bool> hasGUI(true);

//Registries
	static vector<pair<GLFWkeyfun, bool>> keyCBs;
	static vector<pair<GLFWcharfun, bool>> charCBs;
	static vector<pair<GLFWcursorposfun, bool>> cursorposCBs;
	static vector<pair<GLFWcursorenterfun, bool>> cursorenterCBs;
	static vector<pair<GLFWmousebuttonfun, bool>> mousebuttonCBs;
	static vector<pair<GLFWscrollfun, bool>> scrollCBs;
	//static vector<pair<GLFWjoystickfun, bool>> joystickCBs;
	static vector<pair<GLFWdropfun, bool>> dropCBs;
	static vector<pair<GLFWwindowfocusfun, bool>> windowFocusCBs;
	static vector<pair<GLFWwindowsizefun, bool>> windowSizeCBs;
	static vector<GLFWmonitorfun> monitorCBs;

//Create & delete APIs
	void addKeyCB(GLFWkeyfun func, bool noGUIOnly) {
		for (u32 i = 0; i < keyCBs.size(); i++) if (keyCBs[i].first == func) return;
		keyCBs.push_back(make_pair(func, noGUIOnly));
	}
	void removeKeyCB(GLFWkeyfun func) { for (u32 i = 0; i < keyCBs.size(); i++) if (keyCBs[i].first == func) keyCBs.erase(keyCBs.begin() + i); }

	void addCharCB(GLFWcharfun func, bool noGUIOnly) {
		for (u32 i = 0; i < charCBs.size(); i++) if (charCBs[i].first == func) return;
		charCBs.push_back(make_pair(func, noGUIOnly));
	}
	void removeCharCB(GLFWcharfun func) { for (u32 i = 0; i < charCBs.size(); i++) if (charCBs[i].first == func) charCBs.erase(charCBs.begin() + i); }

	void addCursorPosCB(GLFWcursorposfun func, bool noGUIOnly) {
		for (u32 i = 0; i < cursorposCBs.size(); i++) if (cursorposCBs[i].first == func) return;
		cursorposCBs.push_back(make_pair(func, noGUIOnly));
	}
	void removeCursorPosCB(GLFWcursorposfun func) { for (u32 i = 0; i < cursorposCBs.size(); i++) if (cursorposCBs[i].first == func) cursorposCBs.erase(cursorposCBs.begin() + i); }

	void addCursorEnterCB(GLFWcursorenterfun func, bool noGUIOnly) {
		for (u32 i = 0; i < cursorenterCBs.size(); i++) if (cursorenterCBs[i].first == func) return;
		cursorenterCBs.push_back(make_pair(func, noGUIOnly));
	}
	void removeCursorEnterCB(GLFWcursorenterfun func) { for (u32 i = 0; i < cursorenterCBs.size(); i++) if (cursorenterCBs[i].first == func) cursorenterCBs.erase(cursorenterCBs.begin() + i); }

	void addMouseButtonCB(GLFWmousebuttonfun func, bool noGUIOnly) {
		for (u32 i = 0; i < mousebuttonCBs.size(); i++) if (mousebuttonCBs[i].first == func) return;
		mousebuttonCBs.push_back(make_pair(func, noGUIOnly));
	}
	void removeMouseButtonCB(GLFWmousebuttonfun func) { for (u32 i = 0; i < mousebuttonCBs.size(); i++) if (mousebuttonCBs[i].first == func) mousebuttonCBs.erase(mousebuttonCBs.begin() + i); }

	void addScrollCB(GLFWscrollfun func, bool noGUIOnly) {
		for (u32 i = 0; i < scrollCBs.size(); i++) if (scrollCBs[i].first == func) return;
		scrollCBs.push_back(make_pair(func, noGUIOnly));
	}
	void removeScrollCB(GLFWscrollfun func) { for (u32 i = 0; i < scrollCBs.size(); i++) if (scrollCBs[i].first == func) scrollCBs.erase(scrollCBs.begin() + i); }

	//void addJoystickCB(GLFWjoystickfun func, bool noGUIOnly = true);
	//void removeJoystickCB(GLFWjoystickfun func);

	void addDropCB(GLFWdropfun func, bool noGUIOnly) {
		for (u32 i = 0; i < dropCBs.size(); i++) if (dropCBs[i].first == func) return;
		dropCBs.push_back(make_pair(func, noGUIOnly));
	}
	void removeDropCB(GLFWdropfun func) { for (u32 i = 0; i < dropCBs.size(); i++) if (dropCBs[i].first == func) dropCBs.erase(dropCBs.begin() + i); }

	void addWindowFocusCB(GLFWwindowfocusfun func, bool noGUIOnly) {
		for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowFocusCBs[i].first == func) return;
		windowFocusCBs.push_back(make_pair(func, noGUIOnly));
	}
	void removeWindowFocusCB(GLFWwindowfocusfun func) { for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowFocusCBs[i].first == func) windowFocusCBs.erase(windowFocusCBs.begin() + i); }

	void addWindowSizeCB(GLFWwindowsizefun func, bool noGUIOnly) {
		for (u32 i = 0; i < windowSizeCBs.size(); i++) if (windowSizeCBs[i].first == func) return;
		windowSizeCBs.push_back(make_pair(func, noGUIOnly));
	}
	void removeWindowSizeCB(GLFWwindowsizefun func) { for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowSizeCBs[i].first == func) windowSizeCBs.erase(windowSizeCBs.begin() + i); }

	void addMonitorCB(GLFWmonitorfun func) {
		for (u32 i = 0; i < monitorCBs.size(); i++) if (monitorCBs[i] == func) return;
		monitorCBs.push_back(func);
	}
	void removeMonitorCB(GLFWmonitorfun func) { for (u32 i = 0; i < monitorCBs.size(); i++) if (monitorCBs[i] == func) monitorCBs.erase(monitorCBs.begin() + i); }

//Proxies
	static void proxyKeyCB(GLFWwindow* window, int key, int scancode, int action, int mods) { for (u32 i = 0; i < keyCBs.size(); i++) if (!(hasGUI && keyCBs[i].second)) keyCBs[i].first(window, key, scancode, action, mods); }
	static void proxyCharCB(GLFWwindow* window, unsigned int codepoint) { for (u32 i = 0; i < charCBs.size(); i++) if (!(hasGUI && charCBs[i].second)) charCBs[i].first(window, codepoint); }
	static void proxyCursorPosCB(GLFWwindow* window, double xpos, double ypos) { for (u32 i = 0; i < cursorposCBs.size(); i++) if (!(hasGUI && cursorposCBs[i].second)) cursorposCBs[i].first(window, xpos, ypos); }
	static void proxyCursorEnterCB(GLFWwindow* window, int entered) { for (u32 i = 0; i < cursorenterCBs.size(); i++) if (!(hasGUI && cursorenterCBs[i].second)) cursorenterCBs[i].first(window, entered); }
	static void proxyMouseButtonCB(GLFWwindow* window, int button, int action, int mods) { for (u32 i = 0; i < mousebuttonCBs.size(); i++) if (!(hasGUI && mousebuttonCBs[i].second)) mousebuttonCBs[i].first(window, button, action, mods); }
	static void proxyScrollCB(GLFWwindow* window, double xoffset, double yoffset) { for (u32 i = 0; i < scrollCBs.size(); i++) if (!(hasGUI && scrollCBs[i].second)) scrollCBs[i].first(window, xoffset, yoffset); }
	static void proxyDropCB(GLFWwindow* window, int count, const char** paths) { for (u32 i = 0; i < dropCBs.size(); i++) if (!(hasGUI && dropCBs[i].second)) dropCBs[i].first(window, count, paths); }
	static void proxyWindowFocusCB(GLFWwindow* window, int focused) { for (u32 i = 0; i < windowFocusCBs.size(); i++) if (!(hasGUI && windowFocusCBs[i].second)) windowFocusCBs[i].first(window, focused); }
	static void proxyWindowSizeCB(GLFWwindow* window, int width, int height) { for (u32 i = 0; i < windowSizeCBs.size(); i++) if (!(hasGUI && windowSizeCBs[i].second)) windowSizeCBs[i].first(window, width, height); }
	static void proxyMonitorCB(GLFWmonitor* monitor, int event) { for (u32 i = 0; i < monitorCBs.size(); i++) monitorCBs[i](monitor, event); }

//Controls
	void init(GLFWwindow* window) {
		glfwSetKeyCallback(window, proxyKeyCB);
		glfwSetCharCallback(window, proxyCharCB);
		glfwSetCursorPosCallback(window, proxyCursorPosCB);
		glfwSetCursorEnterCallback(window, proxyCursorEnterCB);
		glfwSetMouseButtonCallback(window, proxyMouseButtonCB);
		glfwSetScrollCallback(window, proxyScrollCB);
		glfwSetDropCallback(window, proxyDropCB);
		glfwSetWindowFocusCallback(window, proxyWindowFocusCB);
		glfwSetWindowSizeCallback(window, proxyWindowSizeCB);
		glfwSetMonitorCallback(proxyMonitorCB);
	}
}