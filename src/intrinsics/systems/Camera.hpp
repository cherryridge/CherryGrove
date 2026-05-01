#pragma once
#include <array>
#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <flecs.h>

#include "../components/Camera.hpp"

namespace Systems {
    using std::array;

    [[nodiscard]] inline bool getProjMtx(flecs::entity entity,
        float aspectRatio, array<float, 16>& result
    ) noexcept {
        const auto* camera = entity.try_get<Components::Camera>();
        if (camera != nullptr) {
            bx::mtxProj(result.data(), camera->fov, aspectRatio, camera->nearPlane, camera->farPlane, bgfx::getCaps()->homogeneousDepth, bx::Handedness::Right);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool setFov(flecs::entity entity,
        float fov
    ) noexcept {
        if (entity.has<Components::Camera>()) {
            auto& component = entity.ensure<Components::Camera>();
            component.fov = fov;
            return true;
        }
        return false;
    }
}