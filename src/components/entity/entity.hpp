#pragma once
#include <cstdint>

namespace Components {
    typedef uint32_t u32;
    typedef uint64_t u64;

    struct AccelerationComp {
        double d2x;
        double d2y;
        double d2z;
    };

    struct CameraComp {
        float fov;
        float nearPlane;
        float farPlane;

        CameraComp() = default;
        CameraComp(float fov) {
            this->fov = fov;
            farPlane = 100.0f;
            nearPlane = 0.1f;
        }
    };

    struct CoordinatesComp {
        double x;
        double y;
        double z;
        u64 dimensionId;
    };

    struct HealthComp {
        u32 maxHealth;
        u32 health;
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
        double dx;
        double dy;
        double dz;
    };
}