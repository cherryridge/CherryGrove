#pragma once
#include <array>
#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <entt/entt.hpp>

#include "../components/Camera.hpp"

namespace Systems {
    using std::array;

    [[nodiscard]] inline bool getProjMtx(
        entt::registry& registry, entt::entity entity,
        float aspectRatio, array<float, 16>& result
    ) noexcept {
        const auto* camera = registry.try_get<Components::Camera>(entity);
        if (camera != nullptr) {
            bx::mtxProj(result.data(), camera->fov, aspectRatio, camera->nearPlane, camera->farPlane, bgfx::getCaps()->homogeneousDepth, bx::Handedness::Right);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool setFov(
        entt::registry& registry, entt::entity entity,
        float fov
    ) noexcept {
        if (registry.all_of<Components::Camera>(entity)) {
            registry.patch<Components::Camera>(entity, [fov](Components::Camera& component) {
                component.fov = fov;
            });
            return true;
        }
        return false;
    }
}