#pragma once
#include <bgfx/bgfx.h>
#include <iostream>

class Renderer {
public:
	Renderer();
	~Renderer();
	bool tick();
	bool addShape();
	bool removeShape();
	bool addTexture();
	bool removeTexture();
	bool addShader();
	bool removeShader();
};