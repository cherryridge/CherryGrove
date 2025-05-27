#include <glm/glm.hpp>
#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <entt/entt.hpp>
#include <mutex>

#include "../../simulation/Simulation.hpp"
#include "../Components.hpp"

namespace Components::Camera {
    using namespace Components;

    void getProjMtx(float* result, const entt::entity& entity, float aspectRatio) {
        const auto* camera = Simulation::gameRegistry.try_get<CameraComponent>(entity);
        if(camera) bx::mtxProj(result, camera->fov, aspectRatio, camera->nearPlane, camera->farPlane, bgfx::getCaps()->homogeneousDepth, bx::Handedness::Right);
    }

    void setFov(const entt::entity& entity, float fov) {
        std::lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
        if (Simulation::gameRegistry.all_of<CameraComponent>(entity)) {
            Simulation::gameRegistry.patch<CameraComponent>(entity, [&fov](CameraComponent& component) {
                component.fov = fov;
            });
        }
    }
}