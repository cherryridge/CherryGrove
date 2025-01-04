#pragma once
#include <string>
#include <boost/uuid.hpp>

#include "json/jsontypes.hpp"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

using boost::uuids::uuid, std::string, std::to_string;

namespace PackManager {
	void init();
	void shutdown();

	void refreshPacks(const char* rootDir = "packs");
};