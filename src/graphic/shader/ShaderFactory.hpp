#pragma once
#include <iostream>
#include <bgfx/bgfx.h>

using std::string;

class ShaderFactory{
public:
	ShaderFactory(const char* vsFileName, const char* fsFileName);
	bgfx::ProgramHandle program;
};