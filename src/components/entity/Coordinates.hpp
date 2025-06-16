#pragma once
#include <atomic>
#include <cstdint>
#include <mutex>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../../simulation/Simulation.hpp"
#include "Rotation.hpp"

namespace Components::Coordinates {
    typedef uint32_t u32;
    using std::atomic, std::lock_guard;
    using namespace Components;

    inline atomic<double> baseMovementSpeed(1.0);
    inline atomic<double> baseFlySpeed(0.2);

    inline void forwards(const entt::entity& entity, double delta) noexcept {
        const auto* rotation = Simulation::gameRegistry.try_get<RotationComp>(entity);
        if (rotation && Simulation::gameRegistry.all_of<CoordinatesComp>(entity)) {
            lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
            Simulation::gameRegistry.patch<CoordinatesComp>(entity, [&rotation, &delta](CoordinatesComp& coords) {
                coords.x += baseMovementSpeed * delta * sin(glm::radians(rotation->yaw));
                coords.z += baseMovementSpeed * delta * cos(glm::radians(rotation->yaw));
            });
        }
    }

    inline void backwards(const entt::entity& entity, double delta) noexcept {
        const auto* rotation = Simulation::gameRegistry.try_get<RotationComp>(entity);
        if (rotation && Simulation::gameRegistry.all_of<CoordinatesComp>(entity)) {
            lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
            Simulation::gameRegistry.patch<CoordinatesComp>(entity, [&rotation, &delta](CoordinatesComp& coords) {
                coords.x -= baseMovementSpeed * delta * sin(glm::radians(rotation->yaw));
                coords.z -= baseMovementSpeed * delta * cos(glm::radians(rotation->yaw));
            });
        }
    }

    inline void strafeLeft(const entt::entity& entity, double delta) noexcept {
        const auto* rotation = Simulation::gameRegistry.try_get<RotationComp>(entity);
        if (rotation && Simulation::gameRegistry.all_of<CoordinatesComp>(entity)) {
            lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
            Simulation::gameRegistry.patch<CoordinatesComp>(entity, [&rotation, &delta](CoordinatesComp& coords) {
                coords.x += baseMovementSpeed * delta * sin(glm::radians(rotation->yaw + 90));
                coords.z += baseMovementSpeed * delta * cos(glm::radians(rotation->yaw + 90));
            });
        }
    }

    inline void strafeRight(const entt::entity& entity, double delta) noexcept {
        const auto* rotation = Simulation::gameRegistry.try_get<RotationComp>(entity);
        if (rotation && Simulation::gameRegistry.all_of<CoordinatesComp>(entity)) {
            lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
            Simulation::gameRegistry.patch<CoordinatesComp>(entity, [&rotation, &delta](CoordinatesComp& coords) {
                coords.x -= baseMovementSpeed * delta * sin(glm::radians(rotation->yaw + 90));
                coords.z -= baseMovementSpeed * delta * cos(glm::radians(rotation->yaw + 90));
            });
        }
    }

    inline void fly(const entt::entity& entity, double delta) noexcept {
        if (Simulation::gameRegistry.all_of<CoordinatesComp>(entity)) {
            lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
            Simulation::gameRegistry.patch<CoordinatesComp>(entity, [&delta](CoordinatesComp& coords) {
                coords.y += baseMovementSpeed * delta;
            });
        }
    }
}