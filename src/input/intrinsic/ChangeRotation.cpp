#include <atomic>

#include "../../debug/debug.hpp"

#include "../../components/entity/Rotation.hpp"
#include "../../MainGame.hpp"

#include "../InputHandler.hpp"

#include "ChangeRotation.hpp"

namespace IntrinsicInput {
    using namespace Components::Rotation;

    std::atomic<double> cameraSensitivity = 0.05;
    
    void changeRotationCB(const std::multimap<EventPriority, MouseMove::MMEvent>& events, EventPriority priority, EventFlags flags, const MouseMove::MMEventInfo& info) {
        double dX = (info.newX - info.originX) * cameraSensitivity, dY = (info.newY - info.originY) * cameraSensitivity;
        if (dX == 0) dX = infinity;
        else if (dY == 0) dY = infinity;
        deltaRotation(MainGame::playerEntity, dX, dY);
    }
}