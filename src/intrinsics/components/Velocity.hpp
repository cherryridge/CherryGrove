#pragma once
#include <glm/glm.hpp>

namespace Components {
    struct Velocity {
        //block per tick.
        //Double?: I don't think anything will ever go that fast. Orbital strike cannons' TNTs sent to a reasonable distance are still below the acceptable threshold.
        float dx{0.0f}, dy{0.0f}, dz{0.0f};

        [[nodiscard]] glm::vec3 getGLMVec3() const noexcept { return glm::vec3(dx, dy, dz); }
    };
}