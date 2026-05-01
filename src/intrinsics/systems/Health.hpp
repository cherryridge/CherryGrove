#pragma once
#include <cstdint>
#include <flecs.h>

#include "../components/Health.hpp"

namespace Systems {
    typedef uint32_t u32;

    [[nodiscard]] inline bool updateHealth(flecs::entity entity,
        u32 newHealth
    ) noexcept {
        if (entity.has<Components::Health>()) {
            auto& health = entity.ensure<Components::Health>();
            health.health = newHealth;
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool updateMaxHealth(flecs::entity entity,
        u32 newMaxHealth
    ) noexcept {
        if (entity.has<Components::Health>()) {
            auto& health = entity.ensure<Components::Health>();
            health.maxHealth = newMaxHealth;
            return true;
        }
        return false;
    }
}