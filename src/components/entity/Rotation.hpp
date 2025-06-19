#pragma once
#include <limits>
#include <mutex>
#include <bx/math.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "entity.hpp"
#include "../../simulation/Simulation.hpp"

namespace Components::Rotation {
    using std::lock_guard, std::fmod, glm::vec3, glm::radians, glm::normalize;
    using namespace Components;

    //In world space.
    //Will be configurable soon.
    inline constexpr bx::Vec3 up = { 0.0f, 1.0f, 0.0f };
    //Infinity is dummy value for not changing the field.
    inline constexpr double infinity = std::numeric_limits<double>::infinity();

    inline void setRotation(const entt::entity& entity, double yaw, double pitch) noexcept {
        if (Simulation::gameRegistry.all_of<RotationComp>(entity)) {
            lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
            Simulation::gameRegistry.patch<RotationComp>(entity, [&pitch, &yaw](RotationComp& component) {
                if (yaw != infinity) component.yaw = yaw;
                if (pitch != infinity) component.pitch = pitch;
            });
        }
    }

    inline void deltaRotation(const entt::entity& entity, double dYaw, double dPitch) noexcept {
        if (Simulation::gameRegistry.all_of<RotationComp>(entity)) {
            lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
            Simulation::gameRegistry.patch<RotationComp>(entity, [&dYaw, &dPitch](RotationComp& component) {
                if (dYaw != infinity) {
                    double newYaw = fmod(dYaw + component.yaw, 360.0);
                    if (newYaw < 0) newYaw += 360.0;
                    component.yaw = newYaw;
                }
                if (dPitch != infinity) component.pitch += dPitch;
                if (component.pitch < -89.9999) component.pitch = -89.9999;
                if (component.pitch > 89.9999) component.pitch = 89.9999;
            });
        }
    }

    //Entity must have a `CoordinatesComp` by now.
    inline void getViewMtx(float* result, const entt::entity& entity) noexcept {
        //Use `CoordinatesComp` directly for camera position until `CameraOffsetComp` or `EyeComp` is implemented.
        const auto* coords = Simulation::gameRegistry.try_get<CoordinatesComp>(entity);
        const auto* rotation = Simulation::gameRegistry.try_get<RotationComp>(entity);
        if (coords && rotation) {
            vec3
                lookingAt(
                    //The looking direction.
                    sin(radians(rotation->yaw)) * cos(radians(rotation->pitch)),
                    sin(radians(rotation->pitch)),
                    cos(radians(rotation->yaw)) * cos(radians(rotation->pitch))),
                pos(coords->x, coords->y, coords->z);
            //The normalized looking at coordinates.
            lookingAt = pos + normalize(lookingAt);
            //Using two math libraries to do work is so dumb.
            bx::mtxLookAt(result, reinterpret_cast<bx::Vec3&>(pos), reinterpret_cast<bx::Vec3&>(lookingAt), up, bx::Handedness::Right);
        }
    }
}