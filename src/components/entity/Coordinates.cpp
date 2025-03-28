#include <atomic>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../../MainGame.hpp"
#include "../Components.hpp"

namespace Components::Coordinates {
	using namespace Components;
	using std::lock_guard, std::atomic;

	atomic<double> baseMovementSpeed(1.0);
	atomic<double> baseFlySpeed(0.2);

	void forwards(const entt::entity& entity, double delta) {
		const auto* rotation = MainGame::gameRegistry.try_get<RotationComponent>(entity);
		if (rotation && MainGame::gameRegistry.all_of<CoordinatesComponent>(entity)) {
			lock_guard lock(entity == MainGame::playerEntity ? MainGame::playerMutex : MainGame::registryMutex);
			MainGame::gameRegistry.patch<CoordinatesComponent>(entity, [&rotation, &delta](CoordinatesComponent& coords) {
				coords.x += baseMovementSpeed * delta * sin(glm::radians(rotation->yaw));
				coords.z += baseMovementSpeed * delta * cos(glm::radians(rotation->yaw));
			});
		}
	}

	void backwards(const entt::entity& entity, double delta) {
		const auto* rotation = MainGame::gameRegistry.try_get<RotationComponent>(entity);
		if (rotation && MainGame::gameRegistry.all_of<CoordinatesComponent>(entity)) {
			lock_guard lock(entity == MainGame::playerEntity ? MainGame::playerMutex : MainGame::registryMutex);
			MainGame::gameRegistry.patch<CoordinatesComponent>(entity, [&rotation, &delta](CoordinatesComponent& coords) {
				coords.x -= baseMovementSpeed * delta * sin(glm::radians(rotation->yaw));
				coords.z -= baseMovementSpeed * delta * cos(glm::radians(rotation->yaw));
			});
		}
	}

	void strafeLeft(const entt::entity& entity, double delta) {
		const auto* rotation = MainGame::gameRegistry.try_get<RotationComponent>(entity);
		if (rotation && MainGame::gameRegistry.all_of<CoordinatesComponent>(entity)) {
			lock_guard lock(entity == MainGame::playerEntity ? MainGame::playerMutex : MainGame::registryMutex);
			MainGame::gameRegistry.patch<CoordinatesComponent>(entity, [&rotation, &delta](CoordinatesComponent& coords) {
				coords.x += baseMovementSpeed * delta * sin(glm::radians(rotation->yaw + 90));
				coords.z += baseMovementSpeed * delta * cos(glm::radians(rotation->yaw + 90));
			});
		}
	}

	void strafeRight(const entt::entity& entity, double delta) {
		const auto* rotation = MainGame::gameRegistry.try_get<RotationComponent>(entity);
		if (rotation && MainGame::gameRegistry.all_of<CoordinatesComponent>(entity)) {
			lock_guard lock(entity == MainGame::playerEntity ? MainGame::playerMutex : MainGame::registryMutex);
			MainGame::gameRegistry.patch<CoordinatesComponent>(entity, [&rotation, &delta](CoordinatesComponent& coords) {
				coords.x -= baseMovementSpeed * delta * sin(glm::radians(rotation->yaw + 90));
				coords.z -= baseMovementSpeed * delta * cos(glm::radians(rotation->yaw + 90));
			});
		}
	}

	void fly(const entt::entity& entity, double delta) {
		if (MainGame::gameRegistry.all_of<CoordinatesComponent>(entity)) {
			lock_guard lock(entity == MainGame::playerEntity ? MainGame::playerMutex : MainGame::registryMutex);
			MainGame::gameRegistry.patch<CoordinatesComponent>(entity, [&delta](CoordinatesComponent& coords) {
				coords.y += baseMovementSpeed * delta;
			});
		}
	}
}