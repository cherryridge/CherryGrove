#include <atomic>

#include "../../MainGame.hpp"
#include "../../components/entity/Coordinates.hpp"
#include "../inputBase.hpp"
#include "../sources/boolInput.hpp"
#include "Movement.hpp"

namespace IntrinsicInput {
    using namespace Components::Coordinates;
    using std::atomic;

    atomic<double> moveSensitivity(0.1);
    atomic<double> flySensitivity(0.2);

    void forward(const std::multimap<EventPriority, BoolInput::BIEvent>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        forwards(MainGame::playerEntity, moveSensitivity);
    }

    void backward(const std::multimap<EventPriority, BoolInput::BIEvent>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        backwards(MainGame::playerEntity, moveSensitivity);
    }

    void left(const std::multimap<EventPriority, BoolInput::BIEvent>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        strafeLeft(MainGame::playerEntity, moveSensitivity);
    }

    void right(const std::multimap<EventPriority, BoolInput::BIEvent>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        strafeRight(MainGame::playerEntity, moveSensitivity);
    }

    void up(const std::multimap<EventPriority, BoolInput::BIEvent>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        fly(MainGame::playerEntity, flySensitivity);
    }

    void down(const std::multimap<EventPriority, BoolInput::BIEvent>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
        fly(MainGame::playerEntity, -flySensitivity);
    }
}