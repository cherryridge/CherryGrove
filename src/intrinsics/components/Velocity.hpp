#pragma once
#include <flecs.h>

#include "Acceleration.hpp"

namespace Components {
    struct Velocity {
        //block per tick.
        //Double?: I don't think anything will ever go that fast. Orbital strike cannons' TNTs sent to a reasonable distance are still below the acceptable threshold.
        float dx{0.0f}, dy{0.0f}, dz{0.0f};
    };
}

namespace Components {
    typedef uint32_t u32;

    [[nodiscard]] inline bool updateVelocity(
        flecs::world& registry, flecs::entity entity,
        bool updateX,     bool updateY,     bool updateZ,
        float ndx = 0.0f, float ndy = 0.0f, float ndz = 0.0f
    ) noexcept {
        if (entity.has<Components::Velocity>()) {
            auto& velocity = entity.ensure<Components::Velocity>();
            if (updateX) velocity.dx = ndx;
            if (updateY) velocity.dy = ndy;
            if (updateZ) velocity.dz = ndz;
            return true;
        }
        return false;
    }

    //Entity must have `AccelerationComp`.
    [[nodiscard]] inline bool updateVelocityByAcceleration(flecs::world& registry, flecs::entity entity, u32 deltaTick = 1) noexcept {
        if (!entity.has<Components::Velocity>() || !entity.has<Components::Acceleration>()) return false;
        const auto& acc = entity.get<Components::Acceleration>();
        auto& velocity = entity.ensure<Components::Velocity>();
        velocity.dx += acc.d2x * deltaTick;
        velocity.dy += acc.d2y * deltaTick;
        velocity.dz += acc.d2z * deltaTick;
        return true;
    }
}