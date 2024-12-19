#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "base.hpp"

using std::vector, std::string;

struct ManifestJSON {
	BASE_JSON
	const char* nameSpace;
	const char* uuid;
	const char* name;
	const char* description;
	vector<uint32_t> minEngineVersion;
	vector<uint32_t> version;
	vector<string> features;
};