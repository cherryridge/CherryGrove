#include <atomic>

#include "debug/debug.hpp"
#include "MainGame.hpp"
#include "gui/MainWindow.hpp"
#include "graphic/Renderer.hpp"
#include "sound/Sound.hpp"
#include "gui/Guis.hpp"
#include "pack/Pack.hpp"
#include "input/intrinsic/Escape.hpp"
#include "input/intrinsic/ChangeRotation.hpp"
#include "CherryGrove.hpp"

namespace CherryGrove {
    using std::atomic;

    atomic<bool> isCGAlive(false);

    static void hold();
    static void exit();

    void launch() {
        isCGAlive = true;
        lout << "Main" << flush;

    //Initialize libraries
        lout << "Setting up CherryGrove window & initializing input handler..." << endl;
        MainWindow::initGlfw(2560, 1440, "CherryGrove");
        MainWindow::loadIcon("assets/icons/CherryGrove-trs-64.png");

        lout << "Initializing bgfx & ImGui..." << endl;
        Renderer::start();

        lout << "Initializing SoLoud..." << endl;
        Sound::init();
        Sound::test();

        lout << "Initialzing GUI..." << endl;
        Guis::init();

        lout << "Initializing pack manager..." << endl;
        Pack::init();
        
    //Set up main menu
        Guis::setVisible(Guis::wMainMenu);
        Guis::setVisible(Guis::wCopyright);
        Guis::setVisible(Guis::wVersion);

    //Set up intrinsic inputs
        //todo: Input should be able to be processed by any of the three threads:
        //Main, Renderer, Game
        InputHandler::BoolInput::addBoolInput(InputHandler::BoolInput::BIEType::Start, { "", "escape", 10 }, IntrinsicInput::escapeCB, GLFW_KEY_ESCAPE);

        hold();
    }

    //Main loop.
    //Set `isCGAlive` to `false` directly to exit the program.
    //Check for window updates in `MainWindow::update()` instead.
    static void hold() {
        while (isCGAlive) {
            MainWindow::update();
            if (MainGame::gameStopSignal) {
                MainGame::exit();
                //We must immediately set it to false because we can't guarantee next time here the boolean is already set to false by the Game thread.
                MainGame::gameStopSignal = false;
            }
        }
        exit();
    }

    static void exit() {
        Renderer::waitShutdown();
        if(MainGame::gameStarted) MainGame::exit();
        MainWindow::close();
        Pack::shutdown();
        Sound::shutdown();
        Logger::shutdown();
    }
}