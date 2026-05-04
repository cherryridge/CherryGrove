#pragma once

namespace Components {
    struct EntityMovementProperties {
        bool canWalk{true}, canFly{false}, canSwim{true};
        float walkSpeed{1.0f}, flySpeed{0.2f}, swimSpeed{0.5f};
    };
}