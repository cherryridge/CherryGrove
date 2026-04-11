#pragma once
#include <cstdint>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../components/Coordinates.hpp"
#include "../components/EntityMovementProperties.hpp"
#include "../components/Rotation.hpp"

namespace Systems {
    typedef uint32_t u32;
    typedef int64_t i64;

    [[nodiscard]] inline bool updateEntityCoordinates(
        entt::registry& registry, entt::entity entity,
        bool updateX,    bool updateY,    bool updateZ,
        double nx = 0.0, double ny = 0.0, double nz = 0.0
    ) noexcept {
        if (registry.all_of<Components::EntityCoordinates>(entity)) {
            registry.patch<Components::EntityCoordinates>(entity, [updateX, updateY, updateZ, nx, ny, nz](Components::EntityCoordinates& coordinates) noexcept {
                if (updateX) coordinates.x = nx;
                if (updateY) coordinates.y = ny;
                if (updateZ) coordinates.z = nz;
            });
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool updateBlockCoordinates(
        entt::registry& registry, entt::entity entity,
        bool updateX, bool updateY, bool updateZ,
        i64 nx = 0,   i64 ny = 0,   i64 nz = 0
    ) noexcept {
        if (registry.all_of<Components::BlockCoordinates>(entity)) {
            registry.patch<Components::BlockCoordinates>(entity, [updateX, updateY, updateZ, nx, ny, nz](Components::BlockCoordinates& coordinates) noexcept {
                if (updateX) coordinates.x = nx;
                if (updateY) coordinates.y = ny;
                if (updateZ) coordinates.z = nz;
            });
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool fly(
        entt::registry& registry, entt::entity entity,
        glm::vec3 direction, u32 deltaTicks = 1
    ) noexcept {
        if (registry.all_of<Components::EntityCoordinates, Components::Rotation, Components::EntityMovementProperties>(entity)) {
            const auto& rotation = registry.get<Components::Rotation>(entity);
            const auto& movementProps = registry.get<Components::EntityMovementProperties>(entity);
            registry.patch<Components::EntityCoordinates>(entity, [direction, rotation, &movementProps, deltaTicks](Components::EntityCoordinates& coords) {
                if (movementProps.canFly) {
                    const double
                        yawRad = glm::radians(rotation.yaw),
                        pitchRad = glm::radians(rotation.pitch),
                        sinYaw = glm::sin(yawRad),
                        cosYaw = glm::cos(yawRad),
                        sinPitch = glm::sin(pitchRad),
                        cosPitch = glm::cos(pitchRad),
                        equivalentSpeed = static_cast<double>(movementProps.flySpeed) * deltaTicks;
                    // Forward (direction.z) - follows yaw and pitch
                    coords.x += equivalentSpeed * direction.z * sinYaw * cosPitch;
                    coords.y -= equivalentSpeed * direction.z * sinPitch;
                    coords.z += equivalentSpeed * direction.z * cosYaw * cosPitch;
                    // Right strafe (direction.x) - horizontal only, perpendicular to yaw
                    coords.x -= equivalentSpeed * direction.x * cosYaw;
                    coords.z += equivalentSpeed * direction.x * sinYaw;
                    // Vertical (direction.y) - pure Y axis
                    coords.y += equivalentSpeed * direction.y;
                }
            });
            return true;
        }
        return false;
    }
}