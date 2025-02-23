#pragma once
#include <unordered_map>
#include <glm/glm.hpp>
#include <array>

namespace Components {
	struct CubeFace {
		//{U, V, deltaU, deltaV}
		glm::i16vec4 texCoords;
		float uvRotation;
		uint32_t shaderId;
		uint32_t textureId;
	};

	struct SubCube {
		//All coordinates are relative to block coordinates (corner with least coordinates).
		glm::vec3 origin;
		glm::vec3 size;
		//Selection box origin
		glm::vec3 sOrigin;
		//Selection box size, if no selection box should be displayed, set to (0.0f).
		glm::vec3 sSize;
		glm::vec3 pivot;
		//(rotationX, rotationY, rotationZ), if no rotation, set to (0.0f). 
		glm::vec3 rotation;
		//(up, down, north, east, south, west)
		std::array<CubeFace, 6> faces;
	};

	struct BlockRenderComponent {
		std::unordered_map<uint32_t, SubCube> subcubes;
	};
}