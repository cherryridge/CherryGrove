#pragma once
#include <thread>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
#include <entt/entt.hpp>

#include "../components/Coordinates.hpp"
#include "../components/LoadingSource.hpp"
#include "registry.hpp"

namespace Simulation {
    using std::thread, std::vector, boost::unordered_flat_map;

    struct LoadingZone {
        vector<Components::ChunkCoordinates> loadedChunks;
        vector<entt::entity> objectsInZone;
    };

    inline unordered_flat_map<entt::entity, LoadingZone> loadingZones;
    inline unordered_flat_map<entt::entity, bool> loadingZoneNeedsUpdate;

    inline void updateLoadingZones() noexcept {
        const auto loadingSources = registry.view<const Components::LoadingSource, const Components::ChunkCoordinates>();
        for (const auto& [entity, loadingSource, chunkCoords] : loadingSources.each()) {
            if (!loadingZones.contains(entity)) {
                LoadingZone newZone;
                loadingSource.radius
            }
        }
    }
}