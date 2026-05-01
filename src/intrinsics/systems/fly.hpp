#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <flecs.h>

#include "../components/Coordinates.hpp"
#include "../components/EntityMovementProperties.hpp"
#include "../components/Rotation.hpp"

namespace Systems {
    typedef uint32_t u32;

    [[nodiscard]] inline bool fly(flecs::entity entity,
        glm::vec3 direction, u32 deltaTicks = 1
    ) noexcept {
        if (entity.has<Components::EntityCoordinates>() && entity.has<Components::Rotation>() && entity.has<Components::EntityMovementProperties>()) {
            const auto& rotation = entity.get<Components::Rotation>();
            const auto& movementProps = entity.get<Components::EntityMovementProperties>();
            auto& coords = entity.ensure<Components::EntityCoordinates>();
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
            return true;
        }
        return false;
    }
}