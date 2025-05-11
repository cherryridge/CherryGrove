#include <atomic>
#include <cstdint>
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <GLFW/glfw3.h>
#include <queue>

#include "../input/InputHandler.hpp"
#include "../debug/debug.hpp"
#include "../graphic/Renderer.hpp"
#include "../MainGame.hpp"
#include "../sound/Sound.hpp"
#include "../CherryGrove.hpp"
#include "Guis.hpp"
#include "MainWindow.hpp"

#include <fstream>

namespace MainWindow {
    typedef int32_t i32;
    typedef uint32_t u32;
    using std::thread, std::atomic, std::queue;

    GLFWwindow* window;

//Window icon
    static atomic<bool> iconReadySignal(false);
    static GLFWimage icon;

//Main thread runner
    static queue<void(*)()> taskQueue;

    void initGlfw(u32 width, u32 height, const char* title) {
        if (!glfwInit()) {
            lerr << "[Window] Failed to set up GLFW!" << endl;
            Fatal::exit(Fatal::GLFW_INITIALIZATION_FALILED);
        }
        //Disable OpenGL context.
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            lerr << "[Window] Failed to create GLFW window!" << endl;
            Fatal::exit(Fatal::GLFW_CREATE_WINDOW_FAILED);
        }
        glfwMakeContextCurrent(window);
        InputHandler::init();
    }

    void update() {
        if (glfwWindowShouldClose(window)) {
            CherryGrove::isCGAlive = false;
            return;
        }
        auto size = taskQueue.size();
        for (i32 i = 0; i < size; i++) {
            taskQueue.front()();
            taskQueue.pop();
        }
        if (iconReadySignal) {
            glfwSetWindowIcon(window, 1, &icon);
            iconReadySignal = false;
        }
        glfwWaitEvents();
    }

    void wakeUp() { glfwPostEmptyEvent(); }

    void immediateUpdate() {
        glfwPostEmptyEvent();
        glfwPollEvents();
    }

    void close() {
        glfwSetWindowShouldClose(window, 1);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

//Window icon
    static void s_loadIcon(const char* filePath) {
        stbi_set_flip_vertically_on_load(0);
        i32 iconWidth, iconHeight;
        unsigned char* iconData = stbi_load(filePath, &iconWidth, &iconHeight, nullptr, 4);
        if (iconData) {
            icon.width = iconWidth;
            icon.height = iconHeight;
            icon.pixels = iconData;
            iconReadySignal = true;
        }
        else lerr << "[Window] Load window icon data failed!" << endl;
    }

    void loadIcon(const char* filePath) {
        lout << "Loading window icon from " << filePath << endl;
        thread fileReadThread(&s_loadIcon, filePath);
        fileReadThread.detach();
    }

//Main thread runner
    void runOnMainThread(void(*function)()) { taskQueue.push(function); }
}