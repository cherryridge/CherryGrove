#pragma once
#include <vector>
#include <entt/entt.hpp>

#include "../components/Coordinates.hpp"

namespace Simulation {
    using std::vector;

    struct LoadingZone {
        vector<Components::ChunkCoordinates> loadedChunks;
        vector<entt::entity> objectsInZone;
    };
}