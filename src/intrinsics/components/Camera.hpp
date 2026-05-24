#pragma once
#include <glm/glm.hpp>

namespace Components {
    struct Camera {
        float fov{60.0f}, nearPlane{0.1f}, farPlane{100.0f};
        //The camera's position relative to the entity it's attached to.
        glm::vec3 deltaPosition{0.0f, 0.0f, 0.0f};
        glm::vec3 deltaRotation{0.0f, 0.0f, 0.0f}; //pitch, yaw, roll
    };
}