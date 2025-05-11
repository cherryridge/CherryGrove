#pragma once
#include <limits>
#include <entt/entt.hpp>

namespace Components {
	struct AccelerationComponent {
		double d2x;
		double d2y;
		double d2z;
	};

	namespace Acceleration {
		//Infinity is dummy value for not changing the field.
		inline constexpr double infinity = std::numeric_limits<double>::infinity();

		void setAcceleration(const entt::entity& entity, double nd2x = infinity, double nd2y = infinity, double nd2z = infinity);
	}
}