#pragma once
#include <cstdint>

#include "../components/Acceleration.hpp"
#include "../components/Velocity.hpp"

namespace Systems {
    typedef uint32_t u32;

    [[nodiscard]] inline bool updateAcceleration(flecs::entity entity,
        bool updateX,      bool updateY,      bool updateZ,
        float nd2x = 0.0f, float nd2y = 0.0f, float nd2z = 0.0f
    ) noexcept {
        if (entity.has<Components::Acceleration>()) {
            auto& acceleration = entity.ensure<Components::Acceleration>();
            if (updateX) acceleration.d2x = nd2x;
            if (updateY) acceleration.d2y = nd2y;
            if (updateZ) acceleration.d2z = nd2z;
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool applyAcceleration(flecs::entity entity,
        u32 deltaTicks = 1
    ) noexcept {
        if (entity.has<Components::Acceleration>() && entity.has<Components::Velocity>()) {
            const auto& acc = entity.get<Components::Acceleration>();
            auto& velocity = entity.ensure<Components::Velocity>();
            velocity.dx += acc.d2x * deltaTicks;
            velocity.dy += acc.d2y * deltaTicks;
            velocity.dz += acc.d2z * deltaTicks;
            return true;
        }
        return false;
    }
}