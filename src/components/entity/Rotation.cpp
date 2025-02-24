#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../../gameplay/MainGame.hpp"
#include "../Components.hpp"

namespace Components::Rotation {
	using namespace Components;

	void setRotation(const entt::entity& entity, double yaw, double pitch) {
		MainGame::gameRegistry.patch<RotationComponent>(entity, [&yaw, &pitch](RotationComponent& component) {
			if (yaw != 10000.0) component.yaw = yaw;
			if (pitch != 10000.0) component.pitch = pitch;
		});
	}

	float* getViewMtx(const entt::entity& entity) {
		//Use `CoordinatesComponent` directly for camera position until `CameraOffsetComponent` or `EyeComponent` is implemented.
		auto& coords = MainGame::gameRegistry.get<CoordinatesComponent>(entity);
		auto& rotation = MainGame::gameRegistry.get<RotationComponent>(entity);
		static float result[16];
		glm::vec3
			lookingAt(
				//The looking direction.
				cos(glm::radians(rotation.yaw)) * cos(glm::radians(rotation.pitch)),
				sin(glm::radians(rotation.pitch)),
				sin(glm::radians(rotation.yaw)) * cos(glm::radians(rotation.pitch))
			),
			pos(coords.x, coords.y, coords.z);
		//The normalized looking at coordinates.
		lookingAt = pos + glm::normalize(lookingAt);
		//Using two math libraries to do work is so dumb.
		bx::mtxLookAt(result, reinterpret_cast<bx::Vec3&>(pos), reinterpret_cast<bx::Vec3&>(lookingAt), up, bx::Handedness::Right);
		return result;
	}
}