#pragma once
#include <GLFW/glfw3.h>

namespace MainWindow {
    typedef uint32_t u32;

    extern GLFWwindow* window;

    void initGlfw(u32 width, u32 height, const char* title);
    void update();
    void wakeUp();
    void immediateUpdate();
    void close();

    void loadIcon(const char* filePath);

    void runOnMainThread(void(*callback)());
}