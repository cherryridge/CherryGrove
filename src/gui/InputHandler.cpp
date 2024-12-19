#include <glfw/glfw3.h>
#include <vector>

#include "InputHandler.hpp"

typedef uint32_t u32;

using std::vector;

vector<GLFWmonitorfun> InputHandler::monitorCBs;

InputHandler::InputHandler(GLFWwindow* window) {
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

void InputHandler::addKeyCB(GLFWkeyfun func) {
	for (u32 i = 0; i < keyCBs.size(); i++) if (keyCBs[i] == func) return;
	keyCBs.push_back(func);
}
void InputHandler::removeKeyCB(GLFWkeyfun func) { for (u32 i = 0; i < keyCBs.size(); i++) if (keyCBs[i] == func) keyCBs.erase(keyCBs.begin() + i); }
void InputHandler::proxyKeyCB(GLFWwindow* window, int key, int scancode, int action, int mods) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->keyCBs.size(); i++) instance->keyCBs[i](window, key, scancode, action, mods);
}

void InputHandler::addCharCB(GLFWcharfun func) {
	for (u32 i = 0; i < charCBs.size(); i++) if (charCBs[i] == func) return;
	charCBs.push_back(func);
}
void InputHandler::removeCharCB(GLFWcharfun func) { for (u32 i = 0; i < charCBs.size(); i++) if (charCBs[i] == func) charCBs.erase(charCBs.begin() + i); }
void InputHandler::proxyCharCB(GLFWwindow* window, unsigned int codepoint) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->charCBs.size(); i++) instance->charCBs[i](window, codepoint);
}

void InputHandler::addCursorPosCB(GLFWcursorposfun func) {
	for (u32 i = 0; i < cursorposCBs.size(); i++) if (cursorposCBs[i] == func) return;
	cursorposCBs.push_back(func);
}
void InputHandler::removeCursorPosCB(GLFWcursorposfun func) { for (u32 i = 0; i < cursorposCBs.size(); i++) if (cursorposCBs[i] == func) cursorposCBs.erase(cursorposCBs.begin() + i); }
void InputHandler::proxyCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->cursorposCBs.size(); i++) instance->cursorposCBs[i](window, xpos, ypos);
}

void InputHandler::addCursorEnterCB(GLFWcursorenterfun func) {
	for (u32 i = 0; i < cursorenterCBs.size(); i++) if (cursorenterCBs[i] == func) return;
	cursorenterCBs.push_back(func);
}
void InputHandler::removeCursorEnterCB(GLFWcursorenterfun func) { for (u32 i = 0; i < cursorenterCBs.size(); i++) if (cursorenterCBs[i] == func) cursorenterCBs.erase(cursorenterCBs.begin() + i); }
void InputHandler::proxyCursorEnterCB(GLFWwindow* window, int entered) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->cursorenterCBs.size(); i++) instance->cursorenterCBs[i](window, entered);
}

void InputHandler::addMouseButtonCB(GLFWmousebuttonfun func) {
	for (u32 i = 0; i < mousebuttonCBs.size(); i++) if (mousebuttonCBs[i] == func) return;
	mousebuttonCBs.push_back(func);
}
void InputHandler::removeMouseButtonCB(GLFWmousebuttonfun func) { for (u32 i = 0; i < mousebuttonCBs.size(); i++) if (mousebuttonCBs[i] == func) mousebuttonCBs.erase(mousebuttonCBs.begin() + i); }
void InputHandler::proxyMouseButtonCB(GLFWwindow* window, int button, int action, int mods) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->mousebuttonCBs.size(); i++) instance->mousebuttonCBs[i](window, button, action, mods);
}

void InputHandler::addScrollCB(GLFWscrollfun func) {
	for (u32 i = 0; i < scrollCBs.size(); i++) if (scrollCBs[i] == func) return;
	scrollCBs.push_back(func);
}
void InputHandler::removeScrollCB(GLFWscrollfun func) { for (u32 i = 0; i < scrollCBs.size(); i++) if (scrollCBs[i] == func) scrollCBs.erase(scrollCBs.begin() + i); }
void InputHandler::proxyScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->scrollCBs.size(); i++) instance->scrollCBs[i](window, xoffset, yoffset);
}

void InputHandler::addDropCB(GLFWdropfun func) {
	for (u32 i = 0; i < dropCBs.size(); i++) if (dropCBs[i] == func) return;
	dropCBs.push_back(func);
}
void InputHandler::removeDropCB(GLFWdropfun func) { for (u32 i = 0; i < dropCBs.size(); i++) if (dropCBs[i] == func) dropCBs.erase(dropCBs.begin() + i); }
void InputHandler::proxyDropCB(GLFWwindow* window, int count, const char** paths) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->dropCBs.size(); i++) instance->dropCBs[i](window, count, paths);
}

void InputHandler::addWindowFocusCB(GLFWwindowfocusfun func) {
	for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowFocusCBs[i] == func) return;
	windowFocusCBs.push_back(func);
}
void InputHandler::removeWindowFocusCB(GLFWwindowfocusfun func) { for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowFocusCBs[i] == func) windowFocusCBs.erase(windowFocusCBs.begin() + i); }
void InputHandler::proxyWindowFocusCB(GLFWwindow* window, int focused) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->windowFocusCBs.size(); i++) instance->windowFocusCBs[i](window, focused);
}

void InputHandler::addWindowSizeCB(GLFWwindowsizefun func) {
	for (u32 i = 0; i < windowSizeCBs.size(); i++) if (windowSizeCBs[i] == func) return;
	windowSizeCBs.push_back(func);
}
void InputHandler::removeWindowSizeCB(GLFWwindowsizefun func) { for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowSizeCBs[i] == func) windowSizeCBs.erase(windowSizeCBs.begin() + i); }
void InputHandler::proxyWindowSizeCB(GLFWwindow* window, int width, int height) {
	InputHandler* instance = (InputHandler*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->windowSizeCBs.size(); i++) instance->windowSizeCBs[i](window, width, height);
}

void InputHandler::addMonitorCB(GLFWmonitorfun func) {
	for (u32 i = 0; i < monitorCBs.size(); i++) if (monitorCBs[i] == func) return;
	monitorCBs.push_back(func);
}
void InputHandler::removeMonitorCB(GLFWmonitorfun func) { for (u32 i = 0; i < monitorCBs.size(); i++) if (monitorCBs[i] == func) monitorCBs.erase(monitorCBs.begin() + i); }
void InputHandler::proxyMonitorCB(GLFWmonitor* monitor, int event) {
	for (u32 i = 0; i < monitorCBs.size(); i++) monitorCBs[i](monitor, event);
}