#pragma once
#include <vector>
#include <entt/entt.hpp>

#include "base.hpp"

namespace Simulation {
    using std::vector, entt::registry, entt::entity;

    struct LoadingZone {
        registry ecs;
        vector<ManhattanRange> zones;

        LoadingZone() noexcept = default;
    };
}