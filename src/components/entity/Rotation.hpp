#pragma once
#include <entt/entt.hpp>
#include <bx/math.h>

namespace Components {
	struct RotationComponent {
		//Range: (-90.0, 90.0)
		double yaw;
		//Range: [0.0, 360.0)
		double pitch;
	};

	namespace Rotation {
		//In world space.
		//Will be configurable soon.
		constexpr bx::Vec3 up = { 0.0f, 1.0f, 0.0f };

		//10000.0 is dummy value for not changing the field.
		void setRotation(const entt::registry& registry, const entt::entity& entity, double yaw = 10000.0, double pitch = 10000);

		//array<float, 16>
		//Entity must have a `CoordinatesComponent` by now.
		float* getViewMtx(const entt::registry& registry, const entt::entity& entity);
	}
}