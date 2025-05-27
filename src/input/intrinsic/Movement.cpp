#include <atomic>

#include "../../simulation/Simulation.hpp"
#include "../../components/entity/Coordinates.hpp"
#include "../inputBase.hpp"
#include "../sources/boolInput.hpp"
#include "Movement.hpp"

namespace IntrinsicInput {
    using namespace Components::Coordinates;
    using std::atomic;

    atomic<double> moveSensitivity(0.1);
    atomic<double> flySensitivity(0.2);

    void forward(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        forwards(Simulation::playerEntity, moveSensitivity);
    }

    void backward(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        backwards(Simulation::playerEntity, moveSensitivity);
    }

    void left(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        strafeLeft(Simulation::playerEntity, moveSensitivity);
    }

    void right(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        strafeRight(Simulation::playerEntity, moveSensitivity);
    }

    void up(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        fly(Simulation::playerEntity, flySensitivity);
    }

    void down(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        fly(Simulation::playerEntity, -flySensitivity);
    }
}