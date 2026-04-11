#pragma once
#include <cstdint>
#include <entt/entt.hpp>

#include "../components/Health.hpp"

namespace Systems {
    typedef uint32_t u32;

    [[nodiscard]] inline bool updateHealth(
        entt::registry& registry, entt::entity entity,
        u32 newHealth
    ) noexcept {
        if (registry.all_of<Components::Health>(entity)) {
            registry.patch<Components::Health>(entity, [newHealth](Components::Health& health) noexcept {
                health.health = newHealth;
            });
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool updateMaxHealth(
        entt::registry& registry, entt::entity entity,
        u32 newMaxHealth
    ) noexcept {
        if (registry.all_of<Components::Health>(entity)) {
            registry.patch<Components::Health>(entity, [newMaxHealth](Components::Health& health) noexcept {
                health.maxHealth = newMaxHealth;
            });
            return true;
        }
        return false;
    }
}