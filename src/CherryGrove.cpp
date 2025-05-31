#include <atomic>
#include <latch>

#include "debug/Logger.hpp"
#include "simulation/Simulation.hpp"
#include "graphic/Renderer.hpp"
#include "gui/Window.hpp"
#include "gui/Gui.hpp"
#include "sound/Sound.hpp"
#include "pack/Pack.hpp"
#include "input/intrinsic/Escape.hpp"
#include "CherryGrove.hpp"

namespace CherryGrove {
    using std::atomic, std::latch, std::memory_order_relaxed;

    atomic<bool> isCGAlive(false);
    latch subsystemLatch(3);

    static void hold();
    static void exit();

    void launch() {
        isCGAlive.store(true, memory_order_relaxed);
        lout << "Main" << flush;
    //Read settings
        //todo

    //Initialize systems
        lout << "Setting up CherryGrove window & initializing input handler..." << endl;
        Window::init(2560, 1440, "CherryGrove");

        lout << "Initializing SoLoud..." << endl;
        Sound::init();
        Sound::test();

        lout << "Initializing bgfx & ImGui..." << endl;
        Renderer::init();

        lout << "Initializing pack manager..." << endl;
        Pack::init();

    //Make sure everything is set up
        subsystemLatch.wait();

    //Set up main menu
        Gui::setVisible(Gui::Intrinsics::MainMenu);
        Gui::setVisible(Gui::Intrinsics::Copyright);
        Gui::setVisible(Gui::Intrinsics::Version);

    //Set up intrinsic inputs
        InputHandler::BoolInput::addBoolInput(":escape", 10, IntrinsicInput::escapeCB, InputHandler::BoolInput::ActionTypes::Press, 43);

        hold();
    }

    //Main loop.
    //Set `isCGAlive` to `false` directly to exit the program.
    //Check for windowHandle updates in `Window::update()` instead.
    static void hold() {
        while (isCGAlive) {
            Window::update();
            if (Simulation::gameStopSignal) {
                Simulation::exit();
                //We must immediately set it to false because we can't guarantee next time here the boolean is already set to false by the Game thread.
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