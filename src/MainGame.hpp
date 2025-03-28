#pragma once
#include <atomic>
#include <mutex>
#include <entt/entt.hpp>

namespace MainGame {
	extern std::atomic<bool> gameStarted;
	//Signals the Main thread to call `exit`.
	extern std::atomic<bool> gameStopSignal;
	extern std::atomic<bool> gamePaused;
	extern entt::registry gameRegistry;
	extern entt::entity playerEntity;
	extern std::mutex registryMutex, playerMutex;

	void start();
	//Joins the Game thread. Must NOT be called in game thread.
	void exit();
}