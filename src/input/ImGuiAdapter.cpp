#include <atomic>
#include <GLFW/glfw3.h>

#include "ImGuiAdapter.hpp"

namespace InputHandler {
    std::atomic<bool> sendToImGui(false);

    GLFWkeyfun imGui_keyCB;
    GLFWcharfun imGui_charCB;
    GLFWcursorposfun imGui_cursorPosCB;
    GLFWcursorenterfun imGui_cursorEnterCB;
    GLFWmousebuttonfun imGui_mouseButtonCB;
    GLFWscrollfun imGui_scrollCB;
    GLFWwindowfocusfun imGui_windowFocusCB;
    GLFWmonitorfun imGui_monitorCB;
          
    void submitImGuiCBs(GLFWkeyfun keyCB, GLFWcharfun charCB, GLFWcursorposfun cursorPosCB, GLFWcursorenterfun cursorEnterCB, GLFWmousebuttonfun mouseButtonCB, GLFWscrollfun scrollCB, GLFWwindowfocusfun windowFocusCB, GLFWmonitorfun monitorCB) {
        imGui_keyCB = keyCB;
        imGui_charCB = charCB;
        imGui_cursorPosCB = cursorPosCB;
        imGui_cursorEnterCB = cursorEnterCB;
        imGui_mouseButtonCB = mouseButtonCB;
        imGui_scrollCB = scrollCB;
        imGui_windowFocusCB = windowFocusCB;
        imGui_monitorCB = monitorCB;
    }
}