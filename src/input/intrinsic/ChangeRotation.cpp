#include <atomic>

#include "../../components/entity/Rotation.hpp"
#include "../../simulation/Simulation.hpp"
#include "ChangeRotation.hpp"

namespace IntrinsicInput {
    using namespace Components::Rotation;

    std::atomic<double> cameraSensitivity(0.05);
    
    void changeRotationCB(const std::multimap<EventPriority, MouseMove::Action>& events, EventPriority priority, EventFlags flags, const MouseMove::EventData& info) {
        double dX = info.deltaX * cameraSensitivity, dY = info.deltaY * cameraSensitivity;
        if (dX == 0) dX = infinity;
        else if (dY == 0) dY = infinity;
        deltaRotation(Simulation::playerEntity, dX, dY);
    }
}