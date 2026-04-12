#pragma once
#include <cstdint>
#include <entt/entt.hpp>

#include "../components/Health.hpp"
#include "../simulation/registries.hpp"

namespace Systems {
    typedef uint32_t u32;

    [[nodiscard]] inline bool updateHealth(entt::entity entity,
        u32 newHealth
    ) noexcept {
        if (Simulation::registry.all_of<Components::Health>(entity)) {
            Simulation::registry.patch<Components::Health>(entity, [newHealth](Components::Health& health) noexcept {
                health.health = newHealth;
            });
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool updateMaxHealth(entt::entity entity,
        u32 newMaxHealth
    ) noexcept {
        if (Simulation::registry.all_of<Components::Health>(entity)) {
            Simulation::registry.patch<Components::Health>(entity, [newMaxHealth](Components::Health& health) noexcept {
                health.maxHealth = newMaxHealth;
            });
            return true;
        }
        return false;
    }
}