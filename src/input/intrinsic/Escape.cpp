#include "../../debug/debug.hpp"

#include "../../MainGame.hpp"
#include "../../CherryGrove.hpp"

#include "../InputHandler.hpp"

#include "Escape.hpp"

namespace IntrinsicInput {
	void escapeCB(const std::multimap<EventPriority, BoolInput::BIEvent>& events, EventPriority priority, EventFlags flags, BoolInput::BoolInputID triggerId) {
		if (MainGame::gameStarted) MainGame::gameStopSignal = true;
		else CherryGrove::isCGAlive = false;
	}
}