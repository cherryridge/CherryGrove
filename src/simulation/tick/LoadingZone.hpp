#pragma once
#include <thread>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
#include <flecs.h>

#include "../../intrinsics/components/Coordinates.hpp"
#include "../../intrinsics/components/LoadingSource.hpp"
#include "world.hpp"

namespace Simulation {
    using std::thread, std::vector, boost::unordered_flat_map;

    struct LoadingZone {
        vector<Components::ChunkCoordinates> loadedChunks;
        vector<flecs::entity> objectsInZone;
    };

    inline unordered_flat_map<flecs::entity_t, LoadingZone> loadingZones;
    inline unordered_flat_map<flecs::entity_t, bool> loadingZoneNeedsUpdate;

    inline void updateLoadingZones() noexcept {
        world.each([](flecs::entity entity, const Components::LoadingSource& loadingSource, const Components::ChunkCoordinates& chunkCoords) {
            if (!loadingZones.contains(entity.id())) {
                LoadingZone newZone;
                //todo:
            }
        });
    }
}