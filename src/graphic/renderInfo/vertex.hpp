#pragma once

struct Vertex {
	float x;
	float y;
	float z;
	//Texture coordinates are normalized from `i16` during conversion.
	float u;
	float v;
};

//Debug only.
struct d_ColoredVertex {
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	float a;
};

struct NormalVertex {
	float x;
	float y;
	float z;
	//Texture coordinates are normalized from `i16` during conversion.
	float u;
	float v;
	float nx;
	float ny;
	float nz;
};