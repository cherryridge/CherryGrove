#pragma once
#include <string>
#include <vector>
#include <cstdint>

using string = std::string;

struct PackStatus {

};

struct PackMeta {
	string name;
	string shortName;
	std::vector<string> authors;
	string description;
};

struct PackBehaviorNS {
	string blockNS;
	bool blockNSWarnOnDuplication;
	string structureNS;
	bool structureNSWarnOnDuplication;
	string entityNS;
	bool entityNSWarnOnDuplication;
	string compositeNS;
	bool compositeNSWarnOnDuplication;
	string itemNS;
	bool itemNSWarnOnDuplication;
	string guiNS;
	bool guiNSWarnOnDuplication;
	string achievementNS;
	bool achievementNSWarnOnDuplication;
	string tagNS;
	bool tagNSWarnOnDuplication;
};

struct PackOptionalNS {
	string worldGenNS;
	bool worldGenNSWarnOnDuplication;
};

struct PackResourceNS {
	string textureNS;
	bool textureNSWarnOnDuplication;
	//Actually no plan to support this
	string fontNS;
	bool fontNSWarnOnDuplication;
	string textNS;
	bool textNSWarnOnDuplication;
	string soundNS;
	bool soundNSWarnOnDuplication;
	//Don't add creative tabs here. It should be implemented by creative mode GUI pack.
	string shaderNS;
	bool shaderNSWarnOnDuplication;
};

struct PackConfig {
	uint32_t id;
	bool isGlobal;
	PackMeta meta;
	PackBehaviorNS behaviorNS;
	PackOptionalNS optionalNS;
	PackResourceNS resourceNS;
};