#pragma once
#include <entt/entt.hpp>

namespace Components {
    typedef uint32_t u32;

    struct VelocityComponent {
        double dx;
        double dy;
        double dz;
    };

    namespace Velocity {
        //Entity must have `AccelerationComponent`.
        void updateVByA(const entt::entity& entity, u32 deltaTick = 0);
    }
}