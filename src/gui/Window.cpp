#include <atomic>
#include <cstdint>
#include <functional>
#include <queue>
#include <thread>
#include <backends/imgui_impl_sdl3.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "../input/InputHandler.hpp"
#include "../debug/Logger.hpp"
#include "../debug/Fatal.hpp"
#include "../Main.hpp"
#include "Window.hpp"

namespace Window {
    typedef int32_t i32;
    typedef uint32_t u32;
    using std::thread, std::atomic, std::queue, std::function;

    SDL_Window* windowHandle = nullptr;

//Main thread runner
    static queue<function<void()>> taskQueue;

    void init(u32 width, u32 height, const char* title) noexcept {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
            lerr << "[Window] Failed to set up SDL!" << endl;
            Fatal::exit(Fatal::SDL_INITIALIZATION_FALILED);
        }
        windowHandle = SDL_CreateWindow(title, width, height, SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE);
        if (windowHandle == nullptr) {
            SDL_Quit();
            lerr << "[Window] Failed to create SDL window!" << endl;
            Fatal::exit(Fatal::SDL_CREATE_WINDOW_FAILED);
        }
        auto icon = IMG_Load("assets/icons/CherryGrove-trs-64.png");
        if (icon) SDL_SetWindowIcon(windowHandle, icon);
        else lerr << "[Window] Load window icon data failed!" << endl;
        SDL_DestroySurface(icon);
        InputHandler::init();
        Main::subsystemSetupLatch.count_down();
    }

    void update() noexcept {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(windowHandle))) Main::isCGAlive = false;
            else if(Main::isCGAlive) if(ImGui_ImplSDL3_ProcessEvent(&event)) InputHandler::processTrigger(event);
        }
        auto size = taskQueue.size();
        for (i32 i = 0; i < size; i++) {
            taskQueue.front()();
            taskQueue.pop();
        }
    }

    void close() noexcept {
        SDL_DestroyWindow(windowHandle);
        SDL_Quit();
    }

//Main thread runner
    void runOnMainThread(function<void()> callback) noexcept { taskQueue.push(callback); }
}