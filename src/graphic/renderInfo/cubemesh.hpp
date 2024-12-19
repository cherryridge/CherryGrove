#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include <array>

#include "../../gameplay/base.hpp"

typedef uint32_t u32;

using std::unordered_map, std::array, glm::vec3, glm::ivec3, glm::i16vec4;

struct CubeMesh {
	IDiCoord coordinate;
	unordered_map<u32, SubCubeMesh> subcubes;
};

struct SubCubeMesh {
	//All coordinates are relative to the block bottom center, which is (0.5f, 0.0f, 0.5f) in local coordinate.
	vec3 origin;
	vec3 size;
	//Selection box origin
	vec3 sOrigin;
	//Selection box size
	vec3 sSize;
	vec3 pivot;
	//{rotationX, rotationY, rotationZ}
	vec3 rotation;
	//{up, down, north, east, south, west}
	array<CubeFace, 6> faces;
	u32 shaderId;
};

struct CubeFace {
	//{U, V, deltaU, deltaV}
	i16vec4 texCoords;
	float uvRotation;
	u32 textureId;
};