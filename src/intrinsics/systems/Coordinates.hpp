#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <flecs.h>

#include "../components/Coordinates.hpp"

namespace Systems {
    typedef uint32_t u32;
    typedef int64_t i64;

    [[nodiscard]] inline bool updateEntityCoordinates(flecs::entity entity,
        bool updateX,    bool updateY,    bool updateZ,
        double nx = 0.0, double ny = 0.0, double nz = 0.0
    ) noexcept {
        if (entity.has<Components::EntityCoordinates>()) {
            auto& coordinates = entity.ensure<Components::EntityCoordinates>();
            if (updateX) coordinates.x = nx;
            if (updateY) coordinates.y = ny;
            if (updateZ) coordinates.z = nz;
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool updateBlockCoordinates(flecs::entity entity,
        bool updateX, bool updateY, bool updateZ,
        i64 nx = 0,   i64 ny = 0,   i64 nz = 0
    ) noexcept {
        if (entity.has<Components::BlockCoordinates>()) {
            auto& coordinates = entity.ensure<Components::BlockCoordinates>();
            if (updateX) coordinates.x = nx;
            if (updateY) coordinates.y = ny;
            if (updateZ) coordinates.z = nz;
            return true;
        }
        return false;
    }
}