#pragma once
#include <atomic>
#include <cstdint>
#include <entt/entt.hpp>

namespace Components {
    struct CoordinatesComponent {
        double x;
        double y;
        double z;
        uint32_t dimensionId;
    };

    namespace Coordinates {
        extern std::atomic<double> baseMovementSpeed;
        extern std::atomic<double> baseFlySpeed;

        void forwards(const entt::entity& entity, double delta);
        void backwards(const entt::entity& entity, double delta);
        void strafeLeft(const entt::entity& entity, double delta);
        void strafeRight(const entt::entity& entity, double delta);

        void fly(const entt::entity& entity, double delta);
    }
}