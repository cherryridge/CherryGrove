#pragma once

namespace Components {
    struct Camera {
        float fov{60.0f}, nearPlane{0.1f}, farPlane{100.0f};
    };
}