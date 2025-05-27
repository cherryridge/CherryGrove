#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <mutex>

#include "../../simulation/Simulation.hpp"
#include "../Components.hpp"

namespace Components::Rotation {
    using namespace Components;
    using std::lock_guard;

    void setRotation(const entt::entity& entity, double yaw, double pitch) {
        if (Simulation::gameRegistry.all_of<RotationComponent>(entity)) {
            lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
            Simulation::gameRegistry.patch<RotationComponent>(entity, [&pitch, &yaw](RotationComponent& component) {
                if (yaw != infinity) component.yaw = yaw;
                if (pitch != infinity) component.pitch = pitch;
            });
        }
    }

    void deltaRotation(const entt::entity& entity, double dYaw, double dPitch) {
        if (Simulation::gameRegistry.all_of<RotationComponent>(entity)) {
            lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
            Simulation::gameRegistry.patch<RotationComponent>(entity, [&dYaw, &dPitch](RotationComponent& component) {
                if (dYaw != infinity) {
                    double newYaw = std::fmod(dYaw + component.yaw, 360.0);
                    if (newYaw < 0) newYaw += 360.0;
                    component.yaw = newYaw;
                }
                if (dPitch != infinity) component.pitch += dPitch;
                if (component.pitch < -89.9999) component.pitch = -89.9999;
                if (component.pitch > 89.9999) component.pitch = 89.9999;
            });
        }
    }

    void getViewMtx(float* result, const entt::entity& entity) {
        //Use `CoordinatesComponent` directly for camera position until `CameraOffsetComponent` or `EyeComponent` is implemented.
        const auto* coords = Simulation::gameRegistry.try_get<CoordinatesComponent>(entity);
        const auto* rotation = Simulation::gameRegistry.try_get<RotationComponent>(entity);
        if (coords && rotation) {
            glm::vec3
                lookingAt(
                    //The looking direction.
                    sin(glm::radians(rotation->yaw)) * cos(glm::radians(rotation->pitch)),
                    sin(glm::radians(rotation->pitch)),
                    cos(glm::radians(rotation->yaw)) * cos(glm::radians(rotation->pitch))
                ),
                pos(coords->x, coords->y, coords->z);
            //The normalized looking at coordinates.
            lookingAt = pos + glm::normalize(lookingAt);
            //Using two math libraries to do work is so dumb.
            bx::mtxLookAt(result, reinterpret_cast<bx::Vec3&>(pos), reinterpret_cast<bx::Vec3&>(lookingAt), up, bx::Handedness::Right);
        }
    }
}