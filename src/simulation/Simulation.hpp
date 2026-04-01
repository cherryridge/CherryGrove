#pragma once
#include <atomic>
#include <mutex>
#include <entt/entt.hpp>

namespace Simulation {
    using std::atomic;

    extern atomic<bool> gameStarted;
    //Signals the Main thread to call `exit`.
    extern atomic<bool> gameStopSignal, gamePaused;
    extern entt::registry gameRegistry;
    extern entt::entity playerEntity;
    extern std::mutex registryMutex, playerMutex;

    void start() noexcept;
    void exit() noexcept;
}