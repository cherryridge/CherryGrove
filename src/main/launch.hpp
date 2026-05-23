#pragma once
#include <filesystem>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "../boot/SessionLock.hpp"
#include "../boot/setWorkingDirectory.hpp"
#include "../debug/controller.hpp"
#include "../debug/Fatal.hpp"
#include "../debug/loggers.hpp"
#include "../globalState.hpp"
#include "../graphics/controller.hpp"
#include "../input/boolInput/boolInput.hpp"
#include "../input/InputHandler.hpp"
#include "../intrinsics/actions/Escape.hpp"
#include "../settings/Settings.hpp"
#include "../simulation/controller.hpp"
#include "../sound/API.hpp"
#include "../sound/controller.hpp"
#include "../pack/Pack.hpp"
#include "../util/os/filesystem.hpp"
#include "../window.hpp"
#include "hold.hpp"

namespace Main {
    using std::move, std::cout, std::cerr, std::endl, std::filesystem::current_path, Util::OS::getU8String;

    inline Boot::SessionLock sessionLock;

    //threaded: Main Thread
    //note: This function is definitely pre-logger era, so we can just use `cout`.
    inline void launch(int argc, char** argv) noexcept {
    //Get working directory
        Boot::setWorkingDirectory(argc, argv);
        cout << "Working directory: " << getU8String(current_path()) << endl;

    //Working directory lock (RAII)
        sessionLock = move(Boot::SessionLock("cg.lock"));
        cout << "Session lock acquired." << endl;

    //Settings
        cout << "Reading settings..." << endl;
        if (!Settings::loadSettings()) {
            cerr << "[Main] Failed to load settings!" << endl;
            Debug::exit(Debug::SETTINGS_FAILED_TO_LOAD);
        }

        const auto& settings = Settings::getSettings();

        cout << "Setting up logger..." << endl;
        Debug::init(settings.debug.logging);
        Debug::setThreadName("Main");
        lout << "Hello from Logger!" << nlaf;

    //Mark the starting of CherryGrove
        lout << "Launching CherryGrove..." << nlaf;
        GlobalState::setIsCGAlive(true);

    //Create Main Window
        lout << "Setting up CherryGrove window & initializing input handler..." << nlaf;
        Window::initMainWindow(settings.misc.windowTitleBase.c_str(), settings.graphics.windowWidth, settings.graphics.windowHeight, "assets/icons/CherryGrove-trs-64.png");

    //Initialize InputHandler
        InputHandler::init();

    //Enter the multi-thread era as we are going to spawn the first thread.
        GlobalState::setMultiThreadEra(true);

        lout << "Initializing SoLoud..." << nlaf;
        //This is synchronized, we will wait inside.
        Sound::init();

        {//Debug: Play a.ogg
            Util::Promise<Sound::SoundHandle> p;
            Sound::addSound(&p, "tests/a.ogg", true, true, 1.0f, Sound::FLOAT_INFINITY, Sound::FLOAT_INFINITY);
            const auto handle = p.wait();
            Sound::play(nullptr, handle, {0.0, 0.0, 0.0});
        }

        lout << "Initializing graphics..." << nlaf;
        //Call this on the SDL/main thread before `bgfx::init` so bgfx uses this thread as its render thread.
        bgfx::renderFrame();
        Graphics::init();

        lout << "Initializing pack manager..." << nlaf;
        Pack::init();
    
    //Start Simulation thread
        Simulation::initThread();

    //Set up main menu
        Gui::setVisibility(Gui::Intrinsics::MainMenu, true);
        Gui::setVisibility(Gui::Intrinsics::Copyright, true);
        Gui::setVisibility(Gui::Intrinsics::Version, true);

    //Set up intrinsic inputs
        const auto actionId = InputHandler::BoolInput::add(IntrinsicInput::escapeCB, 10, {InputHandler::BoolInput::BoolInputKind::Down});
        static_cast<void>(InputHandler::BoolInput::addBinding(actionId, InputHandler::BoolInput::KeyCombo(InputHandler::BoolInput::BIInputSource::Keyboard, SDL_SCANCODE_ESCAPE)));

    //Hold main thread
        hold();
    }
}