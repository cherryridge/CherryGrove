#pragma once
#include <cstdint>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../../simulation/registries.hpp"
#include "../components/Coordinates.hpp"
#include "../components/EntityMovementProperties.hpp"
#include "../components/Rotation.hpp"

namespace Systems {
    typedef uint32_t u32;

    [[nodiscard]] inline bool fly(entt::entity entity,
        glm::vec3 direction, u32 deltaTicks = 1
    ) noexcept {
        if (Simulation::registry.all_of<Components::EntityCoordinates, Components::Rotation, Components::EntityMovementProperties>(entity)) {
            const auto& rotation = Simulation::registry.get<Components::Rotation>(entity);
            const auto& movementProps = Simulation::registry.get<Components::EntityMovementProperties>(entity);
            Simulation::registry.patch<Components::EntityCoordinates>(entity, [direction, rotation, &movementProps, deltaTicks](Components::EntityCoordinates& coords) {
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