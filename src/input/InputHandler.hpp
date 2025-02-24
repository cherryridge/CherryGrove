#pragma once
#include <glfw/glfw3.h>
#include <atomic>

namespace InputHandler {
	extern std::atomic<bool> hasGUI;

	void init();
	void shutdown();

	void addKeyCB(GLFWkeyfun func, bool noGUIOnly = true);
	void removeKeyCB(GLFWkeyfun func);

	void addCharCB(GLFWcharfun func, bool noGUIOnly = true);
	void removeCharCB(GLFWcharfun func);

	void addCursorPosCB(GLFWcursorposfun func, bool noGUIOnly = true);
	void removeCursorPosCB(GLFWcursorposfun func);

	void addCursorEnterCB(GLFWcursorenterfun func, bool noGUIOnly = true);
	void removeCursorEnterCB(GLFWcursorenterfun func);

	void addMouseButtonCB(GLFWmousebuttonfun func, bool noGUIOnly = true);
	void removeMouseButtonCB(GLFWmousebuttonfun func);

	void addScrollCB(GLFWscrollfun func, bool noGUIOnly = true);
	void removeScrollCB(GLFWscrollfun func);

	//void addJoystickCB(GLFWjoystickfun func, bool noGUIOnly = true);
	//void removeJoystickCB(GLFWjoystickfun func);

	void addDropCB(GLFWdropfun func, bool noGUIOnly = true);
	void removeDropCB(GLFWdropfun func);

	void addWindowFocusCB(GLFWwindowfocusfun func, bool noGUIOnly = true);
	void removeWindowFocusCB(GLFWwindowfocusfun func);

	void addWindowSizeCB(GLFWwindowsizefun func, bool noGUIOnly = true);
	void removeWindowSizeCB(GLFWwindowsizefun func);

	void addMonitorCB(GLFWmonitorfun func);
	void removeMonitorCB(GLFWmonitorfun func);
}