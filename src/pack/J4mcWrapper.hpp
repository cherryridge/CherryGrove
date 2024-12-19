#pragma once
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "json_schema/manifest.hpp"

typedef uint8_t u8;
typedef uint64_t u64;

namespace J4mcWrapper {
	constexpr u8 ERROR_FILE_NOT_EXIST = 1u;
	constexpr u8 ERROR_CANNOT_OPEN_FILE = 2u;

	void test();

	ManifestJSON parseManifest(const char* filePath);

	template<typename T>
	T parse(const char* filePath, T& targetStruct);

};