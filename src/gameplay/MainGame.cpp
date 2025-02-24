#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <entt/entt.hpp>

#include "../components/Components.hpp"
#include "../debug/debug.hpp"
#include "../gui/Guis.hpp"
#include "../graphic/Renderer.hpp"
#include "MainGame.hpp"

namespace MainGame {
	typedef int32_t i32;
	typedef uint32_t u32;
	using std::atomic, std::thread, std::mutex, std::unique_lock, entt::registry;
	using namespace std::chrono_literals;
	using namespace std::this_thread;

	static void gameLoop();
	static void tick();

	atomic<bool> gameStarted = false;
	atomic<bool> gamePaused = false;
	thread gameThread;
	registry gameRegistry;
	entt::entity playerEntity;
	mutex registryMutex;

	void start() {
		using namespace Components;
		gameStarted = true;
		Guis::setVisible(Guis::wMainMenu, false);
		gameThread = thread(&gameLoop);
		//Temporary code to spawn player entity
		playerEntity = gameRegistry.create();
		gameRegistry.emplace<CameraComponent>(playerEntity, 60.0f);
		gameRegistry.emplace<CoordinatesComponent>(playerEntity, 0.0, 0.0, 0.0, (u32)0);
		gameRegistry.emplace<RotationComponent>(playerEntity, 0.0, 0.0);
		//Test code to spawn a block

	}

	void pause() { gamePaused = true; }

	void exit() {
		gameStarted = false;
		gamePaused = false;
		gameThread.join();
		gameRegistry.destroy(playerEntity);
	}

	//Worker thread, which runs the main game loop at desired rate.
	//Temporary, will be configurable
	constexpr i32 tps = 20;

	static void gameLoop() {
		lout << "Game" << flush;
		while (gameStarted) {
			if (!gamePaused) {
				//Fixed time update not implemented yet
				tick();
			}
		}
	}

	static void tick() {
	//Process player input

	//Update world
		unique_lock lock(registryMutex);
		//Simulates tick
		sleep_for(20ms);
	//Unblock Renderer thread to allow render
		lock.unlock();
	}
}