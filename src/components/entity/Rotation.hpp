#pragma once
#include <bx/math.h>
#include <entt/entt.hpp>

namespace Components {
	struct RotationComponent {
		//Range: [0.0, 360.0).
		//0.0 is facing south (+Z), 90.0 is facing east (+X).
		double yaw;
		//-90.0 is up (+Y in WP), 90.0 is down (-Y in WP).
		//Range: (-90.0, 90.0).
		double pitch;
	};

	namespace Rotation {
		//In world space.
		//Will be configurable soon.
		inline constexpr bx::Vec3 up = { 0.0f, 1.0f, 0.0f };
		//Infinity is dummy value for not changing the field.
		inline constexpr double infinity = std::numeric_limits<double>::infinity();

		void setRotation(const entt::entity& entity, double yaw = infinity, double pitch = infinity);

		void deltaRotation(const entt::entity& entity, double dYaw = infinity, double dPitch = infinity);

		//Entity must have a `CoordinatesComponent` by now.
		void getViewMtx(float* result, const entt::entity& entity);
	}
}