#pragma once
#include <string>
#include <vector>

struct PackMeta {
	std::string name;
	std::string shortName;
	std::vector<std::string> authors;
	std::string description;
};

struct PackBehaviorNS {
	std::string blockNS;
	bool blockNSWarnOnDuplication;
	std::string structureNS;
	bool structureNSWarnOnDuplication;
	std::string entityNS;
	bool entityNSWarnOnDuplication;
	std::string compositeNS;
	bool compositeNSWarnOnDuplication;
	std::string itemNS;
	bool itemNSWarnOnDuplication;
	std::string guiNS;
	bool guiNSWarnOnDuplication;
	std::string achievementNS;
	bool achievementNSWarnOnDuplication;
	std::string tagNS;
	bool tagNSWarnOnDuplication;
};

struct PackOptionalNS {
	std::string worldGenNS;
	bool worldGenNSWarnOnDuplication;
};

struct PackResourceNS {
	std::string textureNS;
	bool textureNSWarnOnDuplication;
	//Actually no plan to support this
	std::string fontNS;
	bool fontNSWarnOnDuplication;
	std::string textNS;
	bool textNSWarnOnDuplication;
	std::string soundNS;
	bool soundNSWarnOnDuplication;
	//Don't add creative tabs here. It should be implemented by creative mode GUI pack.
	std::string shaderNS;
	bool shaderNSWarnOnDuplication;
};

struct PackConfig {
	unsigned int id;
	bool isGlobal;
	PackMeta meta;
	PackBehaviorNS behaviorNS;
	PackOptionalNS optionalNS;
	PackResourceNS resourceNS;
};