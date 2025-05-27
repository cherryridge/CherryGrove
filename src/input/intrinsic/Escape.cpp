
#include "../../simulation/Simulation.hpp"
#include "../../CherryGrove.hpp"
#include "Escape.hpp"

namespace IntrinsicInput {
    void escapeCB(const std::multimap<EventPriority, BoolInput::Action>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        if (Simulation::gameStarted) Simulation::gameStopSignal = true;
        else CherryGrove::isCGAlive = false;
    }
}