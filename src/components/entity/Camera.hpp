#pragma once
#include <bx/math.h>
#include <entt/entt.hpp>

namespace Components {
    struct CameraComponent {
        float fov;
        float nearPlane;
        float farPlane;

        CameraComponent() = default;
        CameraComponent(float fov) {
            this->fov = fov;
            farPlane = 100.0f;
            nearPlane = 0.1f;
        }
    };

    namespace Camera {
        void getProjMtx(float* result, const entt::entity& entity, float aspectRatio);

        void setFov(const entt::entity& entity, float fov);
    }
}