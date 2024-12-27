#include <glfw/glfw3.h>
#include <vector>

#include "InputHandler.hpp"

typedef uint32_t u32;

using std::vector, std::make_pair;

vector<GLFWmonitorfun> InputHandler::monitorCBs;

InputHandler::InputHandler(GLFWwindow* window) {
	hasGUI = true;
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, proxyKeyCB);
	glfwSetCharCallback(window, proxyCharCB);
	glfwSetCursorPosCallback(window, proxyCursorPosCB);
	glfwSetCursorEnterCallback(window, proxyCursorEnterCB);
	glfwSetMouseButtonCallback(window, proxyMouseButtonCB);
	glfwSetScrollCallback(window, proxyScrollCB);
	glfwSetDropCallback(window, proxyDropCB);
	glfwSetWindowFocusCallback(window, proxyWindowFocusCB);
	glfwSetWindowSizeCallback(window, proxyWindowSizeCB);
	//Global
	glfwSetMonitorCallback(proxyMonitorCB);
}


void InputHandler::setHasGUI(bool hasGUI) { this->hasGUI = hasGUI; }


void InputHandler::addKeyCB(GLFWkeyfun func, bool noGUIOnly) {
	for (u32 i = 0; i < keyCBs.size(); i++) if (keyCBs[i].first == func) return;
	keyCBs.push_back(make_pair(func, noGUIOnly));
}

void InputHandler::removeKeyCB(GLFWkeyfun func) { for (u32 i = 0; i < keyCBs.size(); i++) if (keyCBs[i].first == func) keyCBs.erase(keyCBs.begin() + i); }

void InputHandler::proxyKeyCB(GLFWwindow* window, int key, int scancode, int action, int mods) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->keyCBs.size(); i++) if(!(instance->hasGUI && instance->keyCBs[i].second)) instance->keyCBs[i].first(window, key, scancode, action, mods);
}


void InputHandler::addCharCB(GLFWcharfun func, bool noGUIOnly) {
	for (u32 i = 0; i < charCBs.size(); i++) if (charCBs[i].first == func) return;
	charCBs.push_back(make_pair(func, noGUIOnly));
}

void InputHandler::removeCharCB(GLFWcharfun func) { for (u32 i = 0; i < charCBs.size(); i++) if (charCBs[i].first == func) charCBs.erase(charCBs.begin() + i); }

void InputHandler::proxyCharCB(GLFWwindow* window, unsigned int codepoint) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->charCBs.size(); i++) if (!(instance->hasGUI && instance->charCBs[i].second)) instance->charCBs[i].first(window, codepoint);
}


void InputHandler::addCursorPosCB(GLFWcursorposfun func, bool noGUIOnly) {
	for (u32 i = 0; i < cursorposCBs.size(); i++) if (cursorposCBs[i].first == func) return;
	cursorposCBs.push_back(make_pair(func, noGUIOnly));
}

void InputHandler::removeCursorPosCB(GLFWcursorposfun func) { for (u32 i = 0; i < cursorposCBs.size(); i++) if (cursorposCBs[i].first == func) cursorposCBs.erase(cursorposCBs.begin() + i); }

void InputHandler::proxyCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->cursorposCBs.size(); i++) if (!(instance->hasGUI && instance->cursorposCBs[i].second)) instance->cursorposCBs[i].first(window, xpos, ypos);
}


void InputHandler::addCursorEnterCB(GLFWcursorenterfun func, bool noGUIOnly) {
	for (u32 i = 0; i < cursorenterCBs.size(); i++) if (cursorenterCBs[i].first == func) return;
	cursorenterCBs.push_back(make_pair(func, noGUIOnly));
}

void InputHandler::removeCursorEnterCB(GLFWcursorenterfun func) { for (u32 i = 0; i < cursorenterCBs.size(); i++) if (cursorenterCBs[i].first == func) cursorenterCBs.erase(cursorenterCBs.begin() + i); }

void InputHandler::proxyCursorEnterCB(GLFWwindow* window, int entered) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->cursorenterCBs.size(); i++) if (!(instance->hasGUI && instance->cursorenterCBs[i].second)) instance->cursorenterCBs[i].first(window, entered);
}


void InputHandler::addMouseButtonCB(GLFWmousebuttonfun func, bool noGUIOnly) {
	for (u32 i = 0; i < mousebuttonCBs.size(); i++) if (mousebuttonCBs[i].first == func) return;
	mousebuttonCBs.push_back(make_pair(func, noGUIOnly));
}

void InputHandler::removeMouseButtonCB(GLFWmousebuttonfun func) { for (u32 i = 0; i < mousebuttonCBs.size(); i++) if (mousebuttonCBs[i].first == func) mousebuttonCBs.erase(mousebuttonCBs.begin() + i); }

void InputHandler::proxyMouseButtonCB(GLFWwindow* window, int button, int action, int mods) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->mousebuttonCBs.size(); i++) if (!(instance->hasGUI && instance->mousebuttonCBs[i].second)) instance->mousebuttonCBs[i].first(window, button, action, mods);
}


void InputHandler::addScrollCB(GLFWscrollfun func, bool noGUIOnly) {
	for (u32 i = 0; i < scrollCBs.size(); i++) if (scrollCBs[i].first == func) return;
	scrollCBs.push_back(make_pair(func, noGUIOnly));
}

void InputHandler::removeScrollCB(GLFWscrollfun func) { for (u32 i = 0; i < scrollCBs.size(); i++) if (scrollCBs[i].first == func) scrollCBs.erase(scrollCBs.begin() + i); }

void InputHandler::proxyScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->scrollCBs.size(); i++) if (!(instance->hasGUI && instance->scrollCBs[i].second)) instance->scrollCBs[i].first(window, xoffset, yoffset);
}


void InputHandler::addDropCB(GLFWdropfun func, bool noGUIOnly) {
	for (u32 i = 0; i < dropCBs.size(); i++) if (dropCBs[i].first == func) return;
	dropCBs.push_back(make_pair(func, noGUIOnly));
}

void InputHandler::removeDropCB(GLFWdropfun func) { for (u32 i = 0; i < dropCBs.size(); i++) if (dropCBs[i].first == func) dropCBs.erase(dropCBs.begin() + i); }

void InputHandler::proxyDropCB(GLFWwindow* window, int count, const char** paths) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->dropCBs.size(); i++) if (!(instance->hasGUI && instance->dropCBs[i].second)) instance->dropCBs[i].first(window, count, paths);
}


void InputHandler::addWindowFocusCB(GLFWwindowfocusfun func, bool noGUIOnly) {
	for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowFocusCBs[i].first == func) return;
	windowFocusCBs.push_back(make_pair(func, noGUIOnly));
}

void InputHandler::removeWindowFocusCB(GLFWwindowfocusfun func) { for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowFocusCBs[i].first == func) windowFocusCBs.erase(windowFocusCBs.begin() + i); }

void InputHandler::proxyWindowFocusCB(GLFWwindow* window, int focused) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->windowFocusCBs.size(); i++) if (!(instance->hasGUI && instance->windowFocusCBs[i].second)) instance->windowFocusCBs[i].first(window, focused);
}


void InputHandler::addWindowSizeCB(GLFWwindowsizefun func, bool noGUIOnly) {
	for (u32 i = 0; i < windowSizeCBs.size(); i++) if (windowSizeCBs[i].first == func) return;
	windowSizeCBs.push_back(make_pair(func, noGUIOnly));
}

void InputHandler::removeWindowSizeCB(GLFWwindowsizefun func) { for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowSizeCBs[i].first == func) windowSizeCBs.erase(windowSizeCBs.begin() + i); }

void InputHandler::proxyWindowSizeCB(GLFWwindow* window, int width, int height) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->windowSizeCBs.size(); i++) if (!(instance->hasGUI && instance->windowSizeCBs[i].second)) instance->windowSizeCBs[i].first(window, width, height);
}


void InputHandler::addMonitorCB(GLFWmonitorfun func) {
	for (u32 i = 0; i < monitorCBs.size(); i++) if (monitorCBs[i] == func) return;
	monitorCBs.push_back(func);
}

void InputHandler::removeMonitorCB(GLFWmonitorfun func) { for (u32 i = 0; i < monitorCBs.size(); i++) if (monitorCBs[i] == func) monitorCBs.erase(monitorCBs.begin() + i); }

void InputHandler::proxyMonitorCB(GLFWmonitor* monitor, int event) {
	for (u32 i = 0; i < monitorCBs.size(); i++) monitorCBs[i](monitor, event);
}