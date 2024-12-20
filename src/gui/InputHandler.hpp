#pragma once
#include <glfw/glfw3.h>
#include <vector>

using std::vector, std::pair;

class InputHandler {
public:
	InputHandler(GLFWwindow* window);

	void setHasGUI(bool hasGUI);

	void addKeyCB(GLFWkeyfun func, bool noGUIOnly = true);
	void removeKeyCB(GLFWkeyfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<pair<GLFWkeyfun, bool>> keyCBs;
	void addCharCB(GLFWcharfun func, bool noGUIOnly = true);
	void removeCharCB(GLFWcharfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<pair<GLFWcharfun, bool>> charCBs;
	void addCursorPosCB(GLFWcursorposfun func, bool noGUIOnly = true);
	void removeCursorPosCB(GLFWcursorposfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<pair<GLFWcursorposfun, bool>> cursorposCBs;
	void addCursorEnterCB(GLFWcursorenterfun func, bool noGUIOnly = true);
	void removeCursorEnterCB(GLFWcursorenterfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<pair<GLFWcursorenterfun, bool>> cursorenterCBs;
	void addMouseButtonCB(GLFWmousebuttonfun func, bool noGUIOnly = true);
	void removeMouseButtonCB(GLFWmousebuttonfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<pair<GLFWmousebuttonfun, bool>> mousebuttonCBs;
	void addScrollCB(GLFWscrollfun func, bool noGUIOnly = true);
	void removeScrollCB(GLFWscrollfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<pair<GLFWscrollfun, bool>> scrollCBs;
	//void addJoystickCB(GLFWjoystickfun func, bool noGUIOnly = true);
	//void removeJoystickCB(GLFWjoystickfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	//vector<pair<GLFWjoystickfun, bool>> joystickCBs;

	void addDropCB(GLFWdropfun func, bool noGUIOnly = true);
	void removeDropCB(GLFWdropfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<pair<GLFWdropfun, bool>> dropCBs;
	void addWindowFocusCB(GLFWwindowfocusfun func, bool noGUIOnly = true);
	void removeWindowFocusCB(GLFWwindowfocusfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<pair<GLFWwindowfocusfun, bool>> windowFocusCBs;
	void addWindowSizeCB(GLFWwindowsizefun func, bool noGUIOnly = true);
	void removeWindowSizeCB(GLFWwindowsizefun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<pair<GLFWwindowsizefun, bool>> windowSizeCBs;
	//Global
	static void addMonitorCB(GLFWmonitorfun func);
	static void removeMonitorCB(GLFWmonitorfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	static vector<GLFWmonitorfun> monitorCBs;

private:
	bool hasGUI;
	static void proxyKeyCB(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void proxyCharCB(GLFWwindow* window, unsigned int codepoint);
	static void proxyCursorPosCB(GLFWwindow* window, double xpos, double ypos);
	static void proxyCursorEnterCB(GLFWwindow* window, int entered);
	static void proxyMouseButtonCB(GLFWwindow* window, int button, int action, int mods);
	static void proxyScrollCB(GLFWwindow* window, double xoffset, double yoffset);
	//static void proxyJoystickCB(int jid, int event);

	static void proxyDropCB(GLFWwindow* window, int count, const char** paths);
	static void proxyWindowFocusCB(GLFWwindow* window, int focused);
	static void proxyWindowSizeCB(GLFWwindow* window, int width, int height);
	static void proxyMonitorCB(GLFWmonitor* monitor, int event);
};