#pragma once
#include <glm/glm.hpp>

using glm::vec2, glm::vec3, glm::vec4;

struct Vertex {
	//vec3 position;
	float x;
	float y;
	float z;
	//vec2 texCoord;
	//Texture coordinates are normalized from `i16` during conversion.
	float u;
	float v;
};

//Test only.
struct VertexColored {
	//vec3 position;
	float x;
	float y;
	float z;
	//vec4 color;
	float r;
	float g;
	float b;
	float a;
};

struct VertexNormal {
	//vec3 position;
	float x;
	float y;
	float z;
	//vec2 texCoord;
	//Texture coordinates are normalized from `i16` during conversion.
	float u;
	float v;
	//vec3 normal;
	float nx;
	float ny;
	float nz;
};