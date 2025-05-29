#include <atomic>

#include "../../simulation/Simulation.hpp"
#include "../../components/entity/Coordinates.hpp"
#include "../inputBase.hpp"
#include "../sources/boolInput.hpp"
#include "Movement.hpp"

namespace IntrinsicInput {
    using std::atomic;
    using namespace Components::Coordinates;

    atomic<double> moveSensitivity(0.1), flySensitivity(0.2);

    EventFlags forward(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept {
        forwards(Simulation::playerEntity, moveSensitivity);
        return 0;
    }

    EventFlags backward(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept {
        backwards(Simulation::playerEntity, moveSensitivity);
        return 0;
    }

    EventFlags left(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept {
        strafeLeft(Simulation::playerEntity, moveSensitivity);
        return 0;
    }

    EventFlags right(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept {
        strafeRight(Simulation::playerEntity, moveSensitivity);
        return 0;
    }

    EventFlags up(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept {
        fly(Simulation::playerEntity, flySensitivity);
        return 0;
    }

    EventFlags down(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept {
        fly(Simulation::playerEntity, -flySensitivity);
        return 0;
    }
}