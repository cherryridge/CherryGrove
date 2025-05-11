#include <GLFW/glfw3.h>
#include <filesystem>
#include <map>
#include <thread>
#include <unordered_map>

#include "../debug/debug.hpp"
#include "../file/json/Json.hpp"
#include "../file/FileDrop.hpp"
#include "../graphic/Renderer.hpp"
#include "../gui/MainWindow.hpp"
#include "../CherryGrove.hpp"

#include "inputBase.hpp"

#include "ImGuiAdapter.hpp"

#include "sources/boolInput.hpp"
#include "sources/mouseMove.hpp"

#include "InputHandler.hpp"

namespace InputHandler {
    typedef int32_t i32;
    typedef uint32_t u32;

    //The non-GUI part doesn't use this callback.
    static void charCB(GLFWwindow* window, u32 codepoint) {
        if (CherryGrove::isCGAlive && sendToImGui) imGui_charCB(window, codepoint);
    }

    //ImGui doesn't use it?
    static void dropCB(GLFWwindow* window, i32 count, const char** paths) { if (CherryGrove::isCGAlive) FileDrop::processFile(count, paths); }

    //The non-GUI part will be using it very soon. :)
    static void windowFocusCB(GLFWwindow* window, i32 focused) {
        if (CherryGrove::isCGAlive) {
            if (sendToImGui) imGui_windowFocusCB(window, focused);
            //Use it
        }
    }

    //Tell renderer to resize with debounce.
    static void windowSizeCB(GLFWwindow* window, i32 width, i32 height) { Renderer::sizeUpdateSignal = true; }

    static void windowRefreshCB(GLFWwindow* window) {}

    static void monitorCB(GLFWmonitor* monitor, i32 event) { if (CherryGrove::isCGAlive && sendToImGui) imGui_monitorCB(monitor, event); }

    static void windowCloseCB(GLFWwindow* window) { CherryGrove::isCGAlive = false; }

//Process input
    void processInputGame() {
        BoolInput::s_process();
        Scroll::s_process();
    }

    void processInputRenderer() {
        MouseMove::s_process();
    }

//Initialization
    void init() {
    //Register real callbacks
        using MainWindow::window;
        glfwSetCharCallback(window, charCB);
        //glfwSetCharModsCallback();
        glfwSetCursorEnterCallback(window, MouseMove::s_cursorEnterCB);
        glfwSetCursorPosCallback(window, MouseMove::s_cursorPosCB);
        glfwSetDropCallback(window, dropCB);
        //glfwSetErrorCallback();
        //glfwSetFramebufferSizeCallback();
        //glfwSetJoystickCallback();
        glfwSetKeyCallback(window, BoolInput::s_keyCB);
        glfwSetMonitorCallback(monitorCB);
        glfwSetMouseButtonCallback(window, BoolInput::s_mouseButtonCB);
        glfwSetScrollCallback(window, Scroll::s_scrollCB);
        glfwSetWindowCloseCallback(window, windowCloseCB);
        //glfwSetWindowContentScaleCallback();
        glfwSetWindowFocusCallback(window, windowFocusCB);
        //glfwSetWindowIconifyCallback();
        //glfwSetWindowMaximizeCallback();
        //glfwSetWindowPosCallback();
        glfwSetWindowRefreshCallback(window, windowRefreshCB);
        glfwSetWindowSizeCallback(window, windowSizeCB);
    //Initialize input sources
        BoolInput::init();
        MouseMove::init();
        Scroll::init();
    //Get input binding options
        auto _result = Json::getJSON("options.json");
        if (_result.has_value()) {
            const auto& result = _result.value();
            lout << "[InputHandler] Key bindings: " << result.dump(4) << endl;
        }
        else Json::saveJSON("options.json");
    }
}