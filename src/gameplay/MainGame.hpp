#pragma once
#include <atomic>
#include <mutex>
#include <entt/entt.hpp>

namespace MainGame {
	extern std::atomic<bool> gameStarted;
	extern std::atomic<bool> gamePaused;
	extern entt::registry gameRegistry;
	extern entt::entity playerEntity;
	extern std::mutex registryMutex;

	void start();
	void pause();
	void exit();
}