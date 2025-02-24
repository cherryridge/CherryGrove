#pragma once
#include <bx/math.h>
#include <entt/entt.hpp>

namespace Components {
	struct CameraComponent {
		float fov;
		float nearPlane;
		float farPlane;

		CameraComponent() = default;
		CameraComponent(float fov) {
			this->fov = fov;
			farPlane = 100.0f;
			nearPlane = 0.1f;
		}
	};

	namespace Camera {
		//array<float, 16>
		float* getProjMtx(const entt::entity& entity, float aspectRatio);
	}
}