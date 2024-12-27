#pragma once
#include <string>
#include <array>
#include <nlohmann/json.hpp>

#include "pack.hpp"

typedef uint8_t u8;

using std::string, nlohmann::json, std::array;

namespace PackManager {
	void init();
	void shutdown();

	void refreshPacks(const char* rootDir = "packs");
};