#pragma once
#include <array>
#include <bx/bx.h>
#include <bx/math.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "../../simulation/registries.hpp"
#include "../components/Camera.hpp"
#include "../components/Coordinates.hpp"
#include "../components/Rotation.hpp"

namespace Systems {
    using std::array, Simulation::registry;

    //In world space.
    inline constexpr bx::Vec3 bxUp = { 0.0f, 1.0f, 0.0f };

    [[nodiscard]] inline bool updateRotation(entt::entity entity,
        bool updateYaw, bool updatePitch,
        double yaw,     double pitch
    ) noexcept {
        if (registry.all_of<Components::Rotation>(entity)) {
            registry.patch<Components::Rotation>(entity, [updateYaw, updatePitch, yaw, pitch](Components::Rotation& rotation) noexcept {
                if (updateYaw) rotation.yaw = yaw;
                if (updatePitch) rotation.pitch = pitch;
            });
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool deltaRotation(entt::entity entity,
        double dYaw, double dPitch
    ) noexcept {
        if (registry.all_of<Components::Rotation>(entity)) {
            registry.patch<Components::Rotation>(entity, [dYaw, dPitch](Components::Rotation& rotation) noexcept {
                rotation.yaw += dYaw;
                if (rotation.yaw >= 360.0) rotation.yaw -= 360.0;
                else if (rotation.yaw < 0.0) rotation.yaw += 360.0;
                rotation.pitch += dPitch;
                if (rotation.pitch > 89.99999) rotation.pitch = 89.99999;
                else if (rotation.pitch < -89.99999) rotation.pitch = -89.99999;
            });
            return true;
        }
        return false;
    }

    [[nodiscard]] inline glm::vec3 getUnitVecFromRotation(const Components::Rotation& rotation) noexcept {
        return glm::vec3(
            glm::sin(glm::radians(rotation.yaw)) * glm::cos(glm::radians(rotation.pitch)),
            -glm::sin(glm::radians(rotation.pitch)),
            glm::cos(glm::radians(rotation.yaw)) * glm::cos(glm::radians(rotation.pitch))
        );
    }

    namespace Directions {
        [[nodiscard]] inline glm::vec3 forward(const Components::Rotation& rotation) noexcept { return getUnitVecFromRotation(rotation); }

        [[nodiscard]] inline glm::vec3 backwards(const Components::Rotation& rotation) noexcept { return -forward(rotation); }

        [[nodiscard]] inline glm::vec3 left(const Components::Rotation& rotation) noexcept {
            const auto yawRad = glm::radians(rotation.yaw);
            return glm::vec3(
                glm::cos(yawRad),
                0.0f,
                -glm::sin(yawRad)
            );
        }

        [[nodiscard]] inline glm::vec3 right(const Components::Rotation& rotation) noexcept { return -left(rotation); }

        [[nodiscard]] inline glm::vec3 up(const Components::Rotation& rotation) noexcept {
            const auto yawRad = glm::radians(rotation.yaw);
            const auto pitchRad = glm::radians(rotation.pitch);
            return glm::vec3(
                glm::sin(yawRad) * glm::sin(pitchRad),
                glm::cos(pitchRad),
                glm::cos(yawRad) * glm::sin(pitchRad)
            );
        }

        [[nodiscard]] inline glm::vec3 down(const Components::Rotation& rotation) noexcept { return -up(rotation); }
    }

    //Entity must have a `CoordinatesComp` by now.
    [[nodiscard]] inline bool getViewMtx(entt::entity entity,
        array<float, 16>& result
    ) noexcept {
        //Using `EntityCoordinates` directly for camera position until `CameraOffset` or `Eye` is implemented.
        const auto* coords = registry.try_get<Components::EntityCoordinates>(entity);
        const auto* rotation = registry.try_get<Components::Rotation>(entity);
        const auto* camera = registry.try_get<Components::Camera>(entity);
        if (coords && rotation && camera) {
            const glm::vec3 pos(coords->x, coords->y, coords->z);
            const glm::vec3 lookingAt = pos + getUnitVecFromRotation(*rotation);
            //Using two math libraries to do work is so dumb.
            bx::mtxLookAt(result.data(), reinterpret_cast<const bx::Vec3&>(pos), reinterpret_cast<const bx::Vec3&>(lookingAt), bxUp, bx::Handedness::Right);
            return true;
        }
        return false;
    }
}