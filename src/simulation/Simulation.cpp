#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <entt/entt.hpp>

#include "../debug/Logger.hpp"
#include "../input/InputHandler.hpp"
#include "../input/intrinsic/ChangeRotation.hpp"
#include "../input/intrinsic/Movement.hpp"
#include "../components/Components.hpp"
#include "../graphic/TexturePool.hpp"
#include "../gui/Gui.hpp"
#include "../gui/Window.hpp"
#include "Simulation.hpp"

namespace Simulation {
    typedef int32_t i32;
    typedef uint32_t u32;
    using std::atomic, std::thread, std::mutex, std::unique_lock, entt::registry, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds, InputHandler::BoolInput::addBoolInput, InputHandler::MouseMove::addMouseMove, InputHandler::BoolInput::ActionTypes;
    using namespace std::chrono_literals;
    using namespace std::this_thread;
    static void gameLoop() noexcept;
    static void tick() noexcept;

    atomic<bool> gameStarted(false);
    atomic<bool> gameStopSignal(false);
    atomic<bool> gamePaused(false);

    atomic<float> currentTPS(0.0f);
    atomic<float> currentMSPT(0.0f);
    atomic<float> maxTPS(20.0f);

    thread gameThread;
    registry gameRegistry;
    entt::entity playerEntity;
    mutex registryMutex, playerMutex;
    InputHandler::ActionID forward, backward, left, right, up, down, moveCamera;

    void start() {
        gameStarted = true;

        Gui::setVisible(Gui::Intrinsics::MainMenu, false);
        Gui::setVisible(Gui::Intrinsics::Copyright, false);
        Gui::setVisible(Gui::Intrinsics::Version, false);

        Window::runOnMainThread([]() {
            forward = addBoolInput(":forward", 10, IntrinsicInput::forward, ActionTypes::Repeat, 28);
            backward = addBoolInput(":backward", 10, IntrinsicInput::backward, ActionTypes::Repeat, 24);
            left = addBoolInput(":left", 10, IntrinsicInput::left, ActionTypes::Repeat, 6);
            right = addBoolInput(":right", 10, IntrinsicInput::right, ActionTypes::Repeat, 9);
            up = addBoolInput(":up", 10, IntrinsicInput::up, ActionTypes::Repeat, 46);
            down = addBoolInput(":down", 10, IntrinsicInput::down, ActionTypes::Repeat, 165);
            moveCamera = addMouseMove(":moveCamera", 10, IntrinsicInput::changeRotationCB);
            SDL_SetWindowRelativeMouseMode(Window::windowHandle, true);
        });

        //Temporary code to show debug menu
        Gui::setVisible(Gui::Intrinsics::DebugMenu);
        gameThread = thread(&gameLoop);

        //Temporary code to spawn player entity
        using namespace Components;
        playerEntity = gameRegistry.create();
        gameRegistry.emplace<CameraComp>(playerEntity, 60.0f);
        gameRegistry.emplace<CoordinatesComp>(playerEntity, -0.2, -0.5, 1.0, 0u);
        gameRegistry.emplace<RotationComp>(playerEntity, 90.0, 0.0);

        //Test code to spawn a block
        TexturePool::TextureID
            debugpx = TexturePool::addTexture("assets/textures/debug+x.png"),
            debugnx = TexturePool::addTexture("assets/textures/debug-x.png"),
            debugpy = TexturePool::addTexture("assets/textures/debug+y.png"),
            debugny = TexturePool::addTexture("assets/textures/debug-y.png"),
            debugpz = TexturePool::addTexture("assets/textures/debug+z.png"),
            debugnz = TexturePool::addTexture("assets/textures/debug-z.png");
        auto block = gameRegistry.create();
        CubeFace px(glm::uvec2(0, 0), glm::uvec2(16, 16), 0.0f, 1, debugpx);
        CubeFace py(glm::uvec2(0, 0), glm::uvec2(16, 16), 0.0f, 1, debugpy);
        CubeFace pz(glm::uvec2(0, 0), glm::uvec2(16, 16), 0.0f, 1, debugpz);
        CubeFace nx(glm::uvec2(0, 0), glm::uvec2(16, 16), 0.0f, 1, debugnx);
        CubeFace ny(glm::uvec2(0, 0), glm::uvec2(16, 16), 0.0f, 1, debugny);
        CubeFace nz(glm::uvec2(0, 0), glm::uvec2(16, 16), 0.0f, 1, debugnz);
        SubCube sc(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), py, ny, nz, px, pz, nx);
        gameRegistry.emplace<BlockCoordinatesComp>(block, 2, 0, 0, 0u);
        gameRegistry.emplace<BlockRenderComp>(block, sc);
    }

    void exit() {
        //Reset flags
        gameStarted = false;
        gamePaused = false;
        gameStopSignal = false;

        //Clear resources
        gameThread.join();
        gameRegistry.clear();

        //Clear input callbacks (todo: will be changed to use clear())
        Window::runOnMainThread([]() {
            using namespace InputHandler;
            BoolInput::removeBoolInput(forward, ActionTypes::Repeat);
            BoolInput::removeBoolInput(backward, ActionTypes::Repeat);
            BoolInput::removeBoolInput(left, ActionTypes::Repeat);
            BoolInput::removeBoolInput(right, ActionTypes::Repeat);
            BoolInput::removeBoolInput(up, ActionTypes::Repeat);
            BoolInput::removeBoolInput(down, ActionTypes::Repeat);
            MouseMove::removeMouseMove(moveCamera);
            SDL_SetWindowRelativeMouseMode(Window::windowHandle, false);
        });

        //Go back to main menu
        Gui::setVisible(Gui::Intrinsics::DebugMenu, false);
        Gui::setVisible(Gui::Intrinsics::MainMenu);
        Gui::setVisible(Gui::Intrinsics::Copyright);
        Gui::setVisible(Gui::Intrinsics::Version);
    }

    static void gameLoop() noexcept {
        lout << "Game" << flush;
        lout << "Hello from game loop!" << endl;
        while (gameStarted) {
            auto startTime = steady_clock::now();
            if (!gamePaused) tick();
            auto endTime = steady_clock::now();
            auto elapsedTime = duration_cast<microseconds>(endTime - startTime);
            if (elapsedTime < 20000us) sleep_for(20000us - elapsedTime);
        }
        lout << "Game loop terminated!" << endl;
    }

    static void tick() noexcept {
    //Process player input
        InputHandler::processPersist();
    //Update world
        unique_lock lock(registryMutex);
        //Simulates tick
        sleep_for(10ms);
    //Unblock Renderer thread to allow render
        lock.unlock();
    }
}