#pragma once
#include <limits>
#include <entt/entt.hpp>

namespace Components::Acceleration {
    //Infinity is dummy value for not changing the field.
    inline constexpr double infinity = std::numeric_limits<double>::infinity();

    inline void setAcceleration(const entt::entity& entity, double nd2x = infinity, double nd2y = infinity, double nd2z = infinity) noexcept {

    }
}