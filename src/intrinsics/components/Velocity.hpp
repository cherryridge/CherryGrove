#pragma once
#include <entt/entt.hpp>

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
        entt::registry& registry, entt::entity entity,
        bool updateX,     bool updateY,     bool updateZ,
        float ndx = 0.0f, float ndy = 0.0f, float ndz = 0.0f
    ) noexcept {
        if (registry.all_of<Components::Velocity>(entity)) {
            registry.patch<Components::Velocity>(entity, [updateX, updateY, updateZ, ndx, ndy, ndz](Components::Velocity& velocity) noexcept {
                if (updateX) velocity.dx = ndx;
                if (updateY) velocity.dy = ndy;
                if (updateZ) velocity.dz = ndz;
            });
            return true;
        }
        return false;
    }

    //Entity must have `AccelerationComp`.
    [[nodiscard]] inline bool updateVelocityByAcceleration(entt::registry& registry, entt::entity entity, u32 deltaTick = 1) noexcept {
        if (!registry.all_of<Components::Velocity, Components::Acceleration>(entity)) return false;
        const auto& acc = registry.get<Components::Acceleration>(entity);
        registry.patch<Components::Velocity>(entity, [&acc, entity, deltaTick](Components::Velocity& velocity) noexcept {
            velocity.dx += acc.d2x * deltaTick;
            velocity.dy += acc.d2y * deltaTick;
            velocity.dz += acc.d2z * deltaTick;
        });
        return true;
    }
}