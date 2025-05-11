#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <array>
#include <entt/entt.hpp>
#include <GLFW/glfw3.h>

#include "debug/debug.hpp"

#include "input/InputHandler.hpp"
#include "input/intrinsic/ChangeRotation.hpp"
#include "input/intrinsic/Movement.hpp"

#include "components/Components.hpp"
#include "graphic/TexturePool.hpp"
#include "gui/Guis.hpp"
#include "gui/MainWindow.hpp"
#include "graphic/Renderer.hpp"
#include "MainGame.hpp"

namespace MainGame {
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

        Guis::setVisible(Guis::wMainMenu, false);
        Guis::setVisible(Guis::wCopyright, false);
        Guis::setVisible(Guis::wVersion, false);

        MainWindow::runOnMainThread([]() {
            using namespace InputHandler;
            BoolInput::addBoolInput(BoolInput::BIEType::Persist, { "", "forward", 10 }, IntrinsicInput::forward, GLFW_KEY_W);
            BoolInput::addBoolInput(BoolInput::BIEType::Persist, { "", "backward", 10 }, IntrinsicInput::backward, GLFW_KEY_S);
            BoolInput::addBoolInput(BoolInput::BIEType::Persist, { "", "left", 10 }, IntrinsicInput::left, GLFW_KEY_A);
            BoolInput::addBoolInput(BoolInput::BIEType::Persist, { "", "right", 10 }, IntrinsicInput::right, GLFW_KEY_D);
            BoolInput::addBoolInput(BoolInput::BIEType::Persist, { "", "up", 10 }, IntrinsicInput::up, GLFW_KEY_SPACE);
            BoolInput::addBoolInput(BoolInput::BIEType::Persist, { "", "down", 10 }, IntrinsicInput::down, GLFW_KEY_LEFT_SHIFT);
            MouseMove::addMouseMove({ "", "moveCamera", 10 }, IntrinsicInput::changeRotationCB);
            glfwSetInputMode(MainWindow::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        });

        //Temporary code to show debug menu
        Guis::setVisible(Guis::wDebugMenu);
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
        MainWindow::runOnMainThread([]() {
            using namespace InputHandler;
            BoolInput::removeBoolInput(BoolInput::BIEType::Persist, IntrinsicInput::forward);
            BoolInput::removeBoolInput(BoolInput::BIEType::Persist, IntrinsicInput::backward);
            BoolInput::removeBoolInput(BoolInput::BIEType::Persist, IntrinsicInput::left);
            BoolInput::removeBoolInput(BoolInput::BIEType::Persist, IntrinsicInput::right);
            BoolInput::removeBoolInput(BoolInput::BIEType::Persist, IntrinsicInput::up);
            BoolInput::removeBoolInput(BoolInput::BIEType::Persist, IntrinsicInput::down);
            MouseMove::removeMouseMove(IntrinsicInput::changeRotationCB);
            glfwSetInputMode(MainWindow::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        });

        //Go back to main menu
        Guis::setVisible(Guis::wDebugMenu, false);
        Guis::setVisible(Guis::wMainMenu);
        Guis::setVisible(Guis::wCopyright);
        Guis::setVisible(Guis::wVersion);
    }

    static void gameLoop() {
        lout << "Game" << flush;
        lout << "Hello from game loop!" << endl;
        while (gameStarted) {
            auto startTime = steady_clock::now();
            if (!gamePaused) tick();
            auto endTime = steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
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