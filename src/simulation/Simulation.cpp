#include <atomic>
#include <chrono>
#include <thread>
#include <entt/entt.hpp>

#include "../components/Camera.hpp"
#include "../components/Coordinates.hpp"
#include "../components/Rotation.hpp"
#include "../debug/Logger.hpp"
#include "../graphics/gui/Gui.hpp"
#include "../input/InputHandler.hpp"
#include "../input/boolInput/boolInput.hpp"
#include "../input/mouseMove/mouseMove.hpp"
#include "../intrinsics/actions/ChangeRotation.hpp"
#include "../intrinsics/actions/Movement.hpp"
#include "../main/hold.hpp"
#include "registries.hpp"
#include "Simulation.hpp"

namespace Simulation {
    typedef uint64_t u64;
    using std::atomic, std::memory_order_acquire, std::memory_order_release, std::thread, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds, InputHandler::BoolInput::BoolInputKind, InputHandler::MouseMove::SubKind, Util::BitField;
    using namespace std::chrono_literals;
    static void gameLoop() noexcept;
    static void tick() noexcept;

    atomic<bool> gameStarted{false}, gamePaused{false};
    atomic<float> perf_MSPT{0.0f};
    atomic<u64> processingTick{0};

    static thread gameThread;
    static InputHandler::ActionID forward, backward, left, right, up, down, moveCamera;

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
        const entt::entity playerEntity = registry.create();
        registry.emplace<Components::Camera>(playerEntity, 60.0f);
        registry.emplace<Components::EntityCoordinates>(playerEntity, -0.2, -0.5, 1.0, 0u);
        registry.emplace<Components::Rotation>(playerEntity, 90.0, 0.0);
    }

    //threaded: Main thread
    void exit() noexcept {
        //Reset flags
        gameStarted.store(false, memory_order_release);
        gamePaused.store(false, memory_order_release);

        //Clear resources
        gameThread.join();
        registry.clear();

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
            const auto startTime = steady_clock::now();
            if (!gamePaused.load(memory_order_acquire)) tick();
            const auto endTime = steady_clock::now();
            const auto elapsedTime = duration_cast<microseconds>(endTime - startTime);
            perf_MSPT.store(elapsedTime.count() / 1000.0f, memory_order_release);
            if (elapsedTime < 20000us) std::this_thread::sleep_for(20000us - elapsedTime);
        }
        lout << "Game loop terminated!" << endl;
    }

    static void tick() noexcept {
    //1. Update global state and wait for Renderer to finish (if it's not finished already)
        processingTick.fetch_add(1, memory_order_release);

    //2. Send start signal to all region threads

    //3. Wait for all region threads to finish

    //4. Invoke Renderer to render the state

    //5. Update loading zones

    //6. Save to disk

    //7. Process player input
        InputHandler::processTrigger();
        InputHandler::processPersist();
    }
}