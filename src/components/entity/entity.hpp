#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace Components {
    typedef uint32_t u32;
    typedef uint64_t u64;
    using glm::vec3;

    struct AccelerationComp {
        float d2x, d2y, d2z;
    };

    struct CameraComp {
        float fov, nearPlane {0.1f}, farPlane {100.0f};

        CameraComp(float fov = 60.0f) noexcept : fov(fov) {}
    };

    struct CoordinatesComp {
        double x, y, z;
        u64 dimensionId;

        explicit operator vec3() const noexcept {
            return vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
        }
    };

    struct HealthComp {
        u32 maxHealth, health;
    };

    struct RotationComp {
        //Range: [0.0, 360.0).
        //0.0 is facing south (+Z), 90.0 is facing east (+X).
        double yaw;
        //-90.0 is up (+Y in WP), 90.0 is down (-Y in WP).
        //Range: (-90.0, 90.0).
        double pitch;
    };

    struct VelocityComp {
        //block per tick.
        //Double?: I don't think anything will ever go that fast. Orbital strike cannons' TNTs sent to a reasonable distance are still below the acceptable threshold.
        float dx, dy, dz;

        explicit operator vec3() const noexcept {
            return vec3(dx, dy, dz);
        }
    };
}