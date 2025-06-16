#pragma once
#include <mutex>
#include <bx/math.h>
#include <entt/entt.hpp>

#include "entity.hpp"
#include "../../simulation/Simulation.hpp"

namespace Components::Camera {
    using std::lock_guard;
    using namespace Components;

    inline void getProjMtx(float* result, const entt::entity& entity, float aspectRatio) noexcept {
        const auto* camera = Simulation::gameRegistry.try_get<CameraComp>(entity);
        if (camera) bx::mtxProj(result, camera->fov, aspectRatio, camera->nearPlane, camera->farPlane, bgfx::getCaps()->homogeneousDepth, bx::Handedness::Right);
    }

    inline void setFov(const entt::entity& entity, float fov) noexcept {
        std::lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
        if (Simulation::gameRegistry.all_of<CameraComp>(entity)) {
            Simulation::gameRegistry.patch<CameraComp>(entity, [&fov](CameraComp& component) {
                component.fov = fov;
            });
        }
    }
}