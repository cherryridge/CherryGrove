#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <entt/entt.hpp>

#include "../debug/Logger.hpp"
#include "../graphics/gui/Gui.hpp"
#include "../input/InputHandler.hpp"
#include "../input/boolInput/boolInput.hpp"
#include "../input/mouseMove/mouseMove.hpp"
#include "../intrinsics/actions/ChangeRotation.hpp"
#include "../intrinsics/actions/Movement.hpp"
#include "../main/hold.hpp"
#include "Simulation.hpp"

namespace Simulation {
    using std::atomic, std::memory_order_acquire, std::memory_order_release, std::thread, std::mutex, std::unique_lock, entt::registry, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds, InputHandler::BoolInput::BoolInputKind, InputHandler::MouseMove::SubKind, Util::BitField;
    using namespace std::chrono_literals;
    using namespace std::this_thread;
    static void gameLoop() noexcept;
    static void tick() noexcept;

    atomic<bool> gameStarted{false}, gamePaused{false};

    atomic<float> currentTPS{0.0f}, currentMSPT{0.0f}, maxTPS{20.0f};

    thread gameThread;
    registry gameRegistry;
    entt::entity playerEntity;
    mutex registryMutex, playerMutex;
    InputHandler::ActionID forward, backward, left, right, up, down, moveCamera;

    //threaded: Main thread
    void start() noexcept {
        gameStarted.store(true, memory_order_release);
        gamePaused.store(false, memory_order_release);

        Gui::setVisibility(Gui::Intrinsics::MainMenu, false);
        Gui::setVisibility(Gui::Intrinsics::Copyright, false);
        Gui::setVisibility(Gui::Intrinsics::Version, false);

        Main::runOnMainThread.enqueue([]() noexcept {
            forward = InputHandler::BoolInput::add(IntrinsicInput::forward, 10, {BitField<BoolInputKind, BoolInputKind::Count>(BoolInputKind::Persist)});
            backward = InputHandler::BoolInput::add(IntrinsicInput::backward, 10, {BitField<BoolInputKind, BoolInputKind::Count>(BoolInputKind::Persist)});
            left = InputHandler::BoolInput::add(IntrinsicInput::left, 10, {BitField<BoolInputKind, BoolInputKind::Count>(BoolInputKind::Persist)});
            right = InputHandler::BoolInput::add(IntrinsicInput::right, 10, {BitField<BoolInputKind, BoolInputKind::Count>(BoolInputKind::Persist)});
            up = InputHandler::BoolInput::add(IntrinsicInput::up, 10, {BitField<BoolInputKind, BoolInputKind::Count>(BoolInputKind::Persist)});
            down = InputHandler::BoolInput::add(IntrinsicInput::down, 10, {BitField<BoolInputKind, BoolInputKind::Count>(BoolInputKind::Persist)});
            moveCamera = InputHandler::MouseMove::add(IntrinsicInput::changeRotationCB, 10, {BitField<SubKind, SubKind::Count>(SubKind::Persist)});
            InputHandler::setPointerLocked(true);
        });

        //Temporary code to show debug menu
        Gui::setVisibility(Gui::Intrinsics::DebugMenu, true);
        gameThread = thread(gameLoop);

        //Temporary code to spawn player entity
        using namespace Components;
        playerEntity = gameRegistry.create();
        gameRegistry.emplace<CameraComp>(playerEntity, 60.0f);
        gameRegistry.emplace<CoordinatesComp>(playerEntity, -0.2, -0.5, 1.0, 0u);
        gameRegistry.emplace<RotationComp>(playerEntity, 90.0, 0.0);
    }

    //threaded: Main thread
    void exit() noexcept {
        //Reset flags
        gameStarted.store(false, memory_order_release);
        gamePaused.store(false, memory_order_release);

        //Clear resources
        gameThread.join();
        gameRegistry.clear();

        //Clear input callbacks
        Main::runOnMainThread.enqueue([]() noexcept {
            //fixme: Implement the `canDelete` mechanism properly.
            static_cast<void>(InputHandler::BoolInput::remove(forward));
            static_cast<void>(InputHandler::BoolInput::remove(backward));
            static_cast<void>(InputHandler::BoolInput::remove(left));
            static_cast<void>(InputHandler::BoolInput::remove(right));
            static_cast<void>(InputHandler::BoolInput::remove(up));
            static_cast<void>(InputHandler::BoolInput::remove(down));
            static_cast<void>(InputHandler::MouseMove::remove(moveCamera));
            InputHandler::setPointerLocked(false);
        });

        //Go back to main menu
        Gui::setVisibility(Gui::Intrinsics::DebugMenu, false);
        Gui::setVisibility(Gui::Intrinsics::MainMenu, true);
        Gui::setVisibility(Gui::Intrinsics::Copyright, true);
        Gui::setVisibility(Gui::Intrinsics::Version, true);
    }

    static void gameLoop() noexcept {
        lout << "Game" << flush;
        lout << "Hello from game loop!" << endl;
        while (gameStarted.load(memory_order_acquire)) {
            auto startTime = steady_clock::now();
            if (!gamePaused.load(memory_order_acquire)) tick();
            auto endTime = steady_clock::now();
            auto elapsedTime = duration_cast<microseconds>(endTime - startTime);
            if (elapsedTime < 20000us) sleep_for(20000us - elapsedTime);
        }
        lout << "Game loop terminated!" << endl;
    }

    static void tick() noexcept {
    //Process player input
        InputHandler::processTrigger();
        InputHandler::processPersist();
    //Update world
        unique_lock lock(registryMutex);
        //Simulates tick
        sleep_for(10ms);
    //Unblock Renderer thread to allow render
        lock.unlock();
    }
}