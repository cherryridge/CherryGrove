#pragma execution_character_set(push, "utf-8")
#include <atomic>
#include <filesystem>
#include <latch>
#include <SDL3/SDL_main.h>

#include "debug/Logger.hpp"
#include "file/FileLock.hpp"
#include "graphic/Renderer.hpp"
#include "gui/Window.hpp"
#include "gui/Gui.hpp"
#include "input/intrinsic/Escape.hpp"
#include "pack/Pack.hpp"
#include "simulation/Simulation.hpp"
#include "sound/Sound.hpp"
#include "Main.hpp"

typedef int32_t i32;
namespace Main{
    static i32 launch(i32 argc, char** argv);
}
#define SDL_MAIN_HANDLED
i32 SDL_main(i32 argc, char** argv) { return Main::launch(argc, argv); }

namespace Main {
    using std::atomic, std::latch, std::memory_order_relaxed, std::filesystem::current_path, std::filesystem::canonical, std::filesystem::path, std::locale, std::string, std::cout, std::endl;
    static void hold();
    static void exit();

    atomic<bool> isCGAlive(false);
    latch subsystemSetupLatch(3);

    static i32 launch(i32 argc, char** argv) {
    //Execute path
        //Set working directory to parent directory of the executable file
        //todo: Use Physfs
        #ifdef _WIN32
            path exePath(argv[0]);
            path exeDir = canonical(exePath).parent_path();
            current_path(exeDir);
        #endif

    //Logger
        //todo: Use marco & options to determine debug stream destination, e.g. spawn a console and copy stream to it.
        cout << "Setting up logger..." << endl;
        lout << "Main" << flush;
        #ifndef CG_DEBUG_CONSOLE
            Logger::setToFile(true);
        #endif

        //todo: Switch locale
        locale::global(locale("zh_CN.UTF-8"));

        lout << "Hello from Logger!\nWorking directory: " << current_path() << "\nTrying to get unique instance lock..." << endl;

        if (!FileLock::addInstanceLock()) {
            //todo: Focus on the existing instance windowHandle
            lerr << "[Instance Lock] Please don't launch CherryGrove multiple times from one executable. If you need to use multiple instances, copy the whole directory to a new place, or install again at a different location." << endl;
            return Fatal::MISC_MULTIPLE_INSTANCES;
        }

        lout << "Launching CherryGrove..." << endl;
        isCGAlive.store(true, memory_order_relaxed);
    //Read settings
        //todo

    //Initialize systems
        lout << "Setting up CherryGrove window & initializing input handler..." << endl;
        Window::init(2560, 1440, "CherryGrove");

        lout << "Initializing SoLoud..." << endl;
        Sound::init();

        //Debug: Play a.ogg
        auto handle = Sound::addSound("test/a.ogg", true, true, 1.0f, Sound::FLOAT_INFINITY, Sound::FLOAT_INFINITY);
        Sound::play(handle, {0.0, 0.0, 0.0});

        lout << "Initializing bgfx & ImGui..." << endl;
        Renderer::init();

        lout << "Initializing pack manager..." << endl;
        Pack::init();

    //Make sure everything is set up
        subsystemSetupLatch.wait();

    //Set up main menu
        Gui::setVisible(Gui::Intrinsics::MainMenu);
        Gui::setVisible(Gui::Intrinsics::Copyright);
        Gui::setVisible(Gui::Intrinsics::Version);

    //Set up intrinsic inputs
        InputHandler::BoolInput::addBoolInput(":escape", 10, IntrinsicInput::escapeCB, InputHandler::BoolInput::ActionTypes::Press, 43);

        hold();

        FileLock::removeInstanceLock();
        return 0;
    }

    //Main loop.
    //Set `isCGAlive` to `false` directly to exit the program.
    //Check for windowHandle updates in `Window::update()` instead.
    static void hold() {
        while (isCGAlive) {
            Window::update();
            if (Simulation::gameStopSignal) {
                Simulation::exit();
                Simulation::gameStopSignal = false;
            }
        }
        exit();
    }

    static void exit() {
        Renderer::shutdown();
        if(Simulation::gameStarted) Simulation::exit();
        Window::close();
        Pack::shutdown();
        Sound::shutdown();
        Logger::shutdown();
    }
}