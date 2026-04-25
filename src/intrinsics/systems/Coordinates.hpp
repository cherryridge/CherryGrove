#pragma once
#include <cstdint>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../components/Coordinates.hpp"
#include "../simulation/registries.hpp"

namespace Systems {
    typedef uint32_t u32;
    typedef int64_t i64;

    [[nodiscard]] inline bool updateEntityCoordinates(entt::entity entity,
        bool updateX,    bool updateY,    bool updateZ,
        double nx = 0.0, double ny = 0.0, double nz = 0.0
    ) noexcept {
        if (Simulation::registry.all_of<Components::EntityCoordinates>(entity)) {
            Simulation::registry.patch<Components::EntityCoordinates>(entity, [updateX, updateY, updateZ, nx, ny, nz](Components::EntityCoordinates& coordinates) noexcept {
                if (updateX) coordinates.x = nx;
                if (updateY) coordinates.y = ny;
                if (updateZ) coordinates.z = nz;
            });
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool updateBlockCoordinates(entt::entity entity,
        bool updateX, bool updateY, bool updateZ,
        i64 nx = 0,   i64 ny = 0,   i64 nz = 0
    ) noexcept {
        if (Simulation::registry.all_of<Components::BlockCoordinates>(entity)) {
            Simulation::registry.patch<Components::BlockCoordinates>(entity, [updateX, updateY, updateZ, nx, ny, nz](Components::BlockCoordinates& coordinates) noexcept {
                if (updateX) coordinates.x = nx;
                if (updateY) coordinates.y = ny;
                if (updateZ) coordinates.z = nz;
            });
            return true;
        }
        return false;
    }
}