
#include "../../simulation/Simulation.hpp"
#include "../../Main.hpp"
#include "../sources/boolInput.hpp"
#include "../inputBase.hpp"
#include "Escape.hpp"

namespace IntrinsicInput {
    EventFlags escapeCB(const vector<BoolInput::Action>& events, const ActionInfo& info, const BoolInput::EventData& data, EventFlags flags) noexcept {
        if (Simulation::gameStarted) Simulation::gameStopSignal = true;
        else Main::isCGAlive = false;
        return 0;
    }
}