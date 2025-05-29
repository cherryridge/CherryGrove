#include <atomic>

#include "../../components/entity/Rotation.hpp"
#include "../../simulation/Simulation.hpp"
#include "ChangeRotation.hpp"

namespace IntrinsicInput {
    using std::atomic;
    using namespace Components::Rotation;

    atomic<double> cameraSensitivity(0.05);
    
    EventFlags changeRotationCB(const vector<MouseMove::Action>& events, const ActionInfo& info, const MouseMove::EventData& data, EventFlags flags) noexcept {
        double dX = data.deltaX * cameraSensitivity, dY = data.deltaY * cameraSensitivity;
        if (dX == 0) dX = infinity;
        else if (dY == 0) dY = infinity;
        deltaRotation(Simulation::playerEntity, dX, dY);
        return 0;
    }
}