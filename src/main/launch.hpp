#pragma once
#include <filesystem>
#include <iostream>
#include <thread>
#include <bgfx/bgfx.h>

#include "../boot/SessionLock.hpp"
#include "../boot/setWorkingDirectory.hpp"
#include "../debug/controller.hpp"
#include "../debug/Fatal.hpp"
#include "../debug/loggers.hpp"
#include "../globalState.hpp"
#include "../graphics/controller.hpp"
#include "../input/boolInput/boolInput.hpp"
#include "../intrinsics/actions/Escape.hpp"
#include "../settings/Settings.hpp"
#include "../simulation/controller.hpp"
#include "../simulation/runOnSimThread.hpp"
#include "../sound/API.hpp"
#include "../sound/controller.hpp"
#include "../util/os/filesystem.hpp"
#include "../window.hpp"
#include "hold.hpp"

namespace Main {
    typedef uint64_t u64;
    using std::move, std::cout, std::cerr, std::endl, std::filesystem::current_path, std::this_thread::yield, Util::OS::getU8String;

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

    //Enter the multi-thread era.
        GlobalState::setMultiThreadEra(true);

    //Start Simulation thread
        lout << "Initializing simulation..." << nlaf;
        Simulation::initThread();

    //Start Sound thread
        lout << "Initializing sound..." << nlaf;
        Sound::init();

    //Start Graphics thread
        lout << "Initializing graphics..." << nlaf;
        Graphics::init();

    //Wait for all threads to initialize. Main thread has the responsibility of pumping the render thread until it's initialized.
        while (!Simulation::isInitialized() || !Graphics::isInitialized() || !Sound::isInitialized()) bgfx::renderFrame();

    //Play `a.ogg`
        Util::Promise<Sound::SoundHandle> promise;
        Sound::addSound(&promise, "tests/a.ogg", true, true, 1.0f, Sound::FLOAT_INFINITY, Sound::FLOAT_INFINITY);
        const auto handle = promise.wait();
        Sound::play(nullptr, handle, {0.0, 0.0, 0.0});

    //Set up main menu
        Gui::setVisibility(Gui::Intrinsics::MainMenu, true);
        Gui::setVisibility(Gui::Intrinsics::Copyright, true);
        Gui::setVisibility(Gui::Intrinsics::Version, true);

    //Set up intrinsic inputs
        Simulation::runOnSimThread([] () noexcept {
            using namespace IntrinsicInput;
            using namespace InputHandler::BoolInput;

            const auto actionId = add(escapeCB, 10, {BoolInputKind::Down});
            static_cast<void>(addBinding(actionId, KeyCombo(BIInputSource::Keyboard, SDL_SCANCODE_ESCAPE)));
        });

    //Hold main thread
        hold();
    }
}