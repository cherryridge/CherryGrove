#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <vector>

#include "../graphic/texture/TextureFactory.hpp";

typedef uint64_t u64;
typedef uint32_t u32;

using std::vector;

struct blockFace {
	glm::i16vec2 texCoordU;
	glm::i16vec2 texCoordV;
	glm::i16vec2 texCoordU_d;
	glm::i16vec2 texCoordV_d;
	glm::vec3 rotation;
	glm::vec3 scale;
	u32 textureId;
	u32 shaderId;
};

struct cube {
	glm::ivec4 startCoord;
	glm::ivec4 endCoord;
};

class BlockRenderInfo {
public:
	BlockRenderInfo();
	~BlockRenderInfo();

	void render();

private:
	glm::ivec3 worldPosition;
	u64 dimensionId;
	u64 textureId;
	u64 shaderId;
	vector<cube> cubes;
};