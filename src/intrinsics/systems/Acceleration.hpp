#pragma once
#include <cstdint>
#include <entt/entt.hpp>

#include "../components/Acceleration.hpp"
#include "../components/Velocity.hpp"
#include "../simulation/registries.hpp"

namespace Systems {
    typedef uint32_t u32;

    [[nodiscard]] inline bool updateAcceleration(entt::entity entity,
        bool updateX,      bool updateY,      bool updateZ,
        float nd2x = 0.0f, float nd2y = 0.0f, float nd2z = 0.0f
    ) noexcept {
        if (Simulation::registry.all_of<Components::Acceleration>(entity)) {
            Simulation::registry.patch<Components::Acceleration>(entity, [updateX, updateY, updateZ, nd2x, nd2y, nd2z](Components::Acceleration& acceleration) noexcept {
                if (updateX) acceleration.d2x = nd2x;
                if (updateY) acceleration.d2y = nd2y;
                if (updateZ) acceleration.d2z = nd2z;
            });
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool applyAcceleration(entt::entity entity,
        u32 deltaTicks = 1
    ) noexcept {
        if (Simulation::registry.all_of<Components::Acceleration, Components::Velocity>(entity)) {
            const auto& acc = Simulation::registry.get<Components::Acceleration>(entity);
            Simulation::registry.patch<Components::Velocity>(entity, [acc, deltaTicks](Components::Velocity& velocity) noexcept {
                velocity.dx += acc.d2x * deltaTicks;
                velocity.dy += acc.d2y * deltaTicks;
                velocity.dz += acc.d2z * deltaTicks;
            });
            return true;
        }
        return false;
    }
}