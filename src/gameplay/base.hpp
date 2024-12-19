#pragma once
#include <glm/glm.hpp>

typedef uint32_t u32;

struct DiCoord {
	glm::vec3 position;
	u32 dimensionId;
};

struct IDiCoord {
	glm::ivec3 position;
	u32 dimensionId;
};