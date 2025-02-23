#pragma once
#include <atomic>

namespace CherryGrove {
	extern std::atomic<bool> isCGAlive;

	void launch();
}