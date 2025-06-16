#pragma once
#include <entt/entt.hpp>

#include "../../simulation/Simulation.hpp"

namespace Components::Velocity {
    typedef uint32_t u32;
    //Entity must have `AccelerationComp`.
    inline void updateVByA(const entt::entity& entity, u32 deltaTick) noexcept {
        const auto* acceleration = Simulation::gameRegistry.try_get<AccelerationComp>(entity);
        if (acceleration && Simulation::gameRegistry.all_of<VelocityComp>(entity)) {
            std::lock_guard lock(entity == Simulation::playerEntity ? Simulation::playerMutex : Simulation::registryMutex);
            Simulation::gameRegistry.patch<VelocityComp>(entity, [&acceleration, &deltaTick](VelocityComp& velocity) {
                u32 _del = deltaTick ? deltaTick : 1;
                velocity.dx += acceleration->d2x * _del;
                velocity.dx += acceleration->d2y * _del;
                velocity.dx += acceleration->d2z * _del;
            });
        }
    }
}