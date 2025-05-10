#pragma once
#include <atomic>
#include <GLFW/glfw3.h>

namespace InputHandler {
	extern std::atomic<bool> sendToImGui;

	extern GLFWkeyfun imGui_keyCB;
	extern GLFWcharfun imGui_charCB;
	extern GLFWcursorposfun imGui_cursorPosCB;
	extern GLFWcursorenterfun imGui_cursorEnterCB;
	extern GLFWmousebuttonfun imGui_mouseButtonCB;
	extern GLFWscrollfun imGui_scrollCB;
	extern GLFWwindowfocusfun imGui_windowFocusCB;
	extern GLFWmonitorfun imGui_monitorCB;

	void submitImGuiCBs(GLFWkeyfun keyCB, GLFWcharfun charCB, GLFWcursorposfun cursorPosCB, GLFWcursorenterfun cursorEnterCB, GLFWmousebuttonfun mouseButtonCB, GLFWscrollfun scrollCB, GLFWwindowfocusfun windowFocusCB, GLFWmonitorfun monitorCB);
}