#pragma execution_character_set(push, "utf-8")
#include <atomic>
#include <filesystem>
#include <functional>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include "debug/Logger.hpp"
#include "boot/ConfigProvider.hpp"
#include "boot/SessionLock.hpp"
#include "graphics/Renderer.hpp"
#include "gui/Gui.hpp"
#include "intrinsics/actions/Escape.hpp"
#include "input/eventPipeline.hpp"
#include "input/InputHandler.hpp"
#include "pack/Pack.hpp"
#include "settings/Settings.hpp"
#include "simulation/Simulation.hpp"
#include "sound/Sound.hpp"
#include "util/concurrentQueue.hpp"

#include "Main.hpp"

namespace Main {
    static void launch(int argc, char** argv) noexcept;
}
int main(int argc, char** argv) {
    Main::launch(argc, argv);
    return 0;
}

//threaded: Main Thread
namespace Main {
    typedef int32_t i32;
    typedef uint64_t u64;
    using std::atomic, std::memory_order_relaxed, std::memory_order_acquire, std::memory_order_release, std::function, std::filesystem::current_path, std::locale, std::string, std::cout, std::endl, Util::MPSCQueue, InputHandler::nextFrame_M, InputHandler::FramedSDLEvents, InputHandler::eventQueue_M2R, InputHandler::eventQueue_M2S;
    static void hold() noexcept;
    static void exit() noexcept;

    atomic<bool> isCGAlive{false}, multiThreadEra{false};
    SDL_Window* windowHandle{nullptr};
    MPSCQueue<function<void()>> runOnMainThreadMQ;

    static void launch(int argc, char** argv) noexcept {
    //Get working directory
        Boot::setWorkingDirectory(argc, argv);
        cout << "Working directory: " << current_path() << endl;

    //Working directory lock (RAII)
        Boot::SessionLock sessionLock("cg.lock");
        cout << "Session lock acquired." << endl;

    //Settings
        cout << "Reading settings..." << endl;
        if (!Settings::loadSettings(true)) {
            lerr << "[Main] Failed to load settings!" << endl;
            Fatal::exit(Fatal::SETTINGS_FAILED_TO_LOAD);
        }

        cout << "Setting up logger..." << endl;
        Logger::init(Settings::data.debugSettings.logging);
        lout << "Main" << flush;
        lout << "Hello from Logger!" << endl;

    //Locale
        //todo: switch locale
        locale::global(locale("zh_CN.UTF-8"));

    //Mark the starting of CherryGrove
        lout << "Launching CherryGrove..." << endl;
        isCGAlive.store(true, memory_order_relaxed);

    //Create Window
        lout << "Setting up CherryGrove window & initializing input handler..." << endl;
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
            lerr << "[Main] Failed to set up SDL!" << endl;
            Fatal::exit(Fatal::SDL_INITIALIZATION_FALILED);
        }
        windowHandle = SDL_CreateWindow(Settings::data.miscSettings.windowTitleBase.c_str(), Settings::data.graphicSettings.windowWidth, Settings::data.graphicSettings.windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_HIGH_PIXEL_DENSITY);
        if (windowHandle == nullptr) {
            lerr << "[Main] Failed to create SDL window: " << SDL_GetError() << endl;
            SDL_Quit();
            Fatal::exit(Fatal::SDL_CREATE_WINDOW_FAILED);
        }
        auto icon = IMG_Load("assets/icons/CherryGrove-trs-64.png");
        if (icon) SDL_SetWindowIcon(windowHandle, icon);
        else lerr << "[Main] Load window icon data failed!" << endl;
        SDL_DestroySurface(icon);

    //Initialize InputHandler
        InputHandler::init();

    //Enter the multi-thread era as we are going to spawn the first thread.
        multiThreadEra.store(true, memory_order_relaxed);

        lout << "Initializing SoLoud..." << endl;
        //This is synchronized, we will wait inside.
        Sound::init();

        {//Debug: Play a.ogg
            auto handle = Sound::addSound("test/a.ogg", true, true, 1.0f, Sound::FLOAT_INFINITY, Sound::FLOAT_INFINITY);
            Sound::play(handle, {0.0, 0.0, 0.0});
        }

        lout << "Initializing graphics..." << endl;
        //This is synchronized, we will wait inside.
        Renderer::init();

        lout << "Initializing pack manager..." << endl;
        Pack::init(Settings::data.packSettings);

    //Set up main menu
        Gui::setVisible(Gui::Intrinsics::MainMenu);
        Gui::setVisible(Gui::Intrinsics::Copyright);
        Gui::setVisible(Gui::Intrinsics::Version);

    //Set up intrinsic inputs
        InputHandler::BoolInput::addBoolInput(":escape", 10, IntrinsicInput::escapeCB, InputHandler::BoolInput::BIEventType::Press, {InputHandler::BoolInput::InputSource::Keyboard, SDL_Scancode::SDL_SCANCODE_ESCAPE});

        hold();
    }

    //Main loop.
    //Set `isCGAlive` to `false` to exit the program unconditionally.
    static void hold() noexcept {
        SDL_Event event;
        while (isCGAlive.load(memory_order_acquire)) {
        //Populate new input frame
            const auto thisFrame = nextFrame_M.fetch_add(1, memory_order_release);
            FramedSDLEvents frame{1, {}};
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(windowHandle))) {
                    isCGAlive.store(false, memory_order_release);
                    goto stop;
                }
                frame.events.push_back(event);
            }
            eventQueue_M2R.enqueue(frame);
            if (Simulation::gameStarted.load(memory_order_acquire)) eventQueue_M2S.enqueue(frame);
        //end
        //Process Simulation stop signal
            //todo: Make a better one.
            if (Simulation::gameStopSignal.load(memory_order_acquire)) {
                Simulation::exit();
                Simulation::gameStopSignal.store(false, memory_order_release);
            }
        //end
        //Drain `runOnMainThread` MPSCQueue
            //todo: set a maximum number of tasks per loop?
            function<void()> task;
            while (runOnMainThreadMQ.dequeue(task)) task();
        //end
        }
        stop: exit();
    }

    static void exit() noexcept {
        Renderer::shutdown();
        if (Simulation::gameStarted) Simulation::exit();
        SDL_DestroyWindow(windowHandle);
        SDL_Quit();
        Pack::shutdown();
        Sound::shutdown();
        Logger::shutdown();
    }
}