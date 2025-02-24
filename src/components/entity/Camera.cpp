#include <glm/glm.hpp>
#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <entt/entt.hpp>

#include "../../gameplay/MainGame.hpp"
#include "../Components.hpp"

namespace Components::Camera {
	using namespace Components;

	float* getProjMtx(const entt::entity& entity, float aspectRatio) {
		static float result[16];
		auto& camera = MainGame::gameRegistry.get<CameraComponent>(entity);
		bx::mtxProj(result, camera.fov, aspectRatio, camera.nearPlane, camera.farPlane, bgfx::getCaps()->homogeneousDepth);
		return result;
	}
}