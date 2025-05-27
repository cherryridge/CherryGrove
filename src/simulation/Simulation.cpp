#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
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
    using std::atomic, std::thread, std::mutex, std::unique_lock, entt::registry, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds;
    using namespace std::chrono_literals;
    using namespace std::this_thread;

    static void gameLoop();
    static void tick();

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

    void start() {
        gameStarted = true;

        Gui::setVisible(Gui::wMainMenu, false);
        Gui::setVisible(Gui::wCopyright, false);
        Gui::setVisible(Gui::wVersion, false);

        Window::runOnMainThread([]() {
            using namespace InputHandler;
            BoolInput::addBoolInput(BoolInput::ActionTypes::Repeat, { "", "forward", 10 }, IntrinsicInput::forward, GLFW_KEY_W);
            BoolInput::addBoolInput(BoolInput::ActionTypes::Repeat, { "", "backward", 10 }, IntrinsicInput::backward, GLFW_KEY_S);
            BoolInput::addBoolInput(BoolInput::ActionTypes::Repeat, { "", "left", 10 }, IntrinsicInput::left, GLFW_KEY_A);
            BoolInput::addBoolInput(BoolInput::ActionTypes::Repeat, { "", "right", 10 }, IntrinsicInput::right, GLFW_KEY_D);
            BoolInput::addBoolInput(BoolInput::ActionTypes::Repeat, { "", "up", 10 }, IntrinsicInput::up, GLFW_KEY_SPACE);
            BoolInput::addBoolInput(BoolInput::ActionTypes::Repeat, { "", "down", 10 }, IntrinsicInput::down, GLFW_KEY_LEFT_SHIFT);
            MouseMove::addMouseMove({ "", "moveCamera", 10 }, IntrinsicInput::changeRotationCB);
            glfwSetInputMode(Window::windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        });

        //Temporary code to show debug menu
        Gui::setVisible(Gui::wDebugMenu);
        gameThread = thread(&gameLoop);

        //Temporary code to spawn player entity
        using namespace Components;
        playerEntity = gameRegistry.create();
        gameRegistry.emplace<CameraComponent>(playerEntity, 60.0f);
        gameRegistry.emplace<CoordinatesComponent>(playerEntity, -0.2, -0.5, 1.0, 0u);
        gameRegistry.emplace<RotationComponent>(playerEntity, 90.0, 0.0);

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
        gameRegistry.emplace<BlockCoordinatesComponent>(block, 2, 0, 0, 0u);
        gameRegistry.emplace<BlockRenderComponent>(block, sc);
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
            BoolInput::removeBoolInput(BoolInput::ActionTypes::Repeat, IntrinsicInput::forward);
            BoolInput::removeBoolInput(BoolInput::ActionTypes::Repeat, IntrinsicInput::backward);
            BoolInput::removeBoolInput(BoolInput::ActionTypes::Repeat, IntrinsicInput::left);
            BoolInput::removeBoolInput(BoolInput::ActionTypes::Repeat, IntrinsicInput::right);
            BoolInput::removeBoolInput(BoolInput::ActionTypes::Repeat, IntrinsicInput::up);
            BoolInput::removeBoolInput(BoolInput::ActionTypes::Repeat, IntrinsicInput::down);
            MouseMove::removeMouseMove(IntrinsicInput::changeRotationCB);
            glfwSetInputMode(Window::windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        });

        //Go back to main menu
        Gui::setVisible(Gui::wDebugMenu, false);
        Gui::setVisible(Gui::wMainMenu);
        Gui::setVisible(Gui::wCopyright);
        Gui::setVisible(Gui::wVersion);
    }

    static void gameLoop() {
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

    static void tick() {
    //Process player input
        InputHandler::processInputGame();
    //Update world
        unique_lock lock(registryMutex);
        //Simulates tick
        sleep_for(10ms);
    //Unblock Renderer thread to allow render
        lock.unlock();
    }
}