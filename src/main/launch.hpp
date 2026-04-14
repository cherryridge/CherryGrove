#pragma once
#include <atomic>
#include <filesystem>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "../boot/ConfigProvider.hpp"
#include "../boot/SessionLock.hpp"
#include "../debug/Fatal.hpp"
#include "../globalState.hpp"
#include "../graphics/controller.hpp"
#include "../intrinsics/actions/Escape.hpp"
#include "../input/boolInput/boolInput.hpp"
#include "../input/InputHandler.hpp"
#include "../settings/Settings.hpp"
#include "../sound/Sound.hpp"
#include "../pack/Pack.hpp"
#include "../util/BitField.hpp"
#include "../window.hpp"
#include "hold.hpp"

namespace Main {
    using std::memory_order_relaxed, std::cout, std::endl, std::filesystem::current_path, Util::BitField;

    inline Boot::SessionLock sessionLock;

    //threaded: Main Thread
    //note: This function is definitely pre-logger era, so we can just use `cout`.
    inline void launch(int argc, char** argv) noexcept {
    //Get working directory
        Boot::setWorkingDirectory(argc, argv);
        cout << "Working directory: " << current_path() << endl;

    //Working directory lock (RAII)
        sessionLock = Boot::SessionLock("cg.lock");
        cout << "Session lock acquired." << endl;

    //Settings
        cout << "Reading settings..." << endl;
        if (!Settings::loadSettings()) {
            lerr << "[Main] Failed to load settings!" << endl;
            Fatal::exit(Fatal::SETTINGS_FAILED_TO_LOAD);
        }

        const auto& settings = Settings::getSettings();

        cout << "Setting up logger..." << endl;
        Logger::init(settings.debug.logging);
        lout << "Main" << flush;
        lout << "Hello from Logger!" << endl;

    //Mark the starting of CherryGrove
        lout << "Launching CherryGrove..." << endl;
        GlobalState::isCGAlive.store(true, memory_order_relaxed);

    //Create Main Window
        lout << "Setting up CherryGrove window & initializing input handler..." << endl;
        Window::initMainWindow(settings.misc.windowTitleBase.c_str(), settings.graphics.windowWidth, settings.graphics.windowHeight, "assets/icons/CherryGrove-trs-64.png");

    //Initialize InputHandler
        InputHandler::init();

    //Enter the multi-thread era as we are going to spawn the first thread.
        GlobalState::multiThreadEra.store(true, memory_order_relaxed);

        lout << "Initializing SoLoud..." << endl;
        //This is synchronized, we will wait inside.
        Sound::init();

        {//Debug: Play a.ogg
            auto handle = Sound::addSound("tests/a.ogg", true, true, 1.0f, Sound::FLOAT_INFINITY, Sound::FLOAT_INFINITY);
            static_cast<void>(Sound::play(handle, {0.0, 0.0, 0.0}));
        }

        lout << "Initializing graphics..." << endl;
        //This is synchronized, we will wait inside.
        Graphics::init();

        lout << "Initializing pack manager..." << endl;
        Pack::init();

    //Set up main menu
        Gui::setVisibility(Gui::Intrinsics::MainMenu, true);
        Gui::setVisibility(Gui::Intrinsics::Copyright, true);
        Gui::setVisibility(Gui::Intrinsics::Version, true);

    //Set up intrinsic inputs
        static_cast<void>(InputHandler::BoolInput::add(IntrinsicInput::escapeCB, 10, {
            .allowedKinds = BitField<InputHandler::BoolInput::BoolInputKind, InputHandler::BoolInput::BoolInputKind::Count>(InputHandler::BoolInput::BoolInputKind::Down)
        }));

        hold();
    }
}