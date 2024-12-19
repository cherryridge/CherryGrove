#pragma once

#include "V8Wrapper.hpp"
#include "J4mcWrapper.hpp"

namespace PackManager {
	void init(const char* _workingDirectory);
	void shutdown();
};