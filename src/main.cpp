#include <windows.h>
#include <bgfx/bgfx.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#include <iostream>

#include "graphic/shader/shader.h"

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920

#ifdef NDEBUG
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#else
int main(int argc, char* argv[]) {
#endif
    GLFWwindow* window;
    if (!glfwInit()) return -1;
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cherry Grove", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    bgfx::Init config;
    bgfx::PlatformData pdata;
    pdata.nwh = glfwGetWin32Window(window);
    config.type = bgfx::RendererType::Count;
    config.resolution.width = WINDOW_WIDTH;
    config.resolution.height = WINDOW_HEIGHT;
    config.resolution.reset = BGFX_RESET_VSYNC;
    config.platformData = pdata;
    bgfx::init(config);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    unsigned int counter = 0;
    while (!glfwWindowShouldClose(window)) {
        bgfx::touch(0);
        bgfx::frame();
        glfwPollEvents();
        counter++;
        if (counter % 10 == 0) std::cout << counter / 10 << std::endl;
    }
    glfwTerminate();
    glfwDestroyWindow(window);
    bgfx::shutdown();
    return 0;
}