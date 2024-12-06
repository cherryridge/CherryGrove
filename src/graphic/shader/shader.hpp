#pragma once
#include <iostream>
#include <bgfx/bgfx.h>

using std::string;

class Shader{
public:
	Shader(const char* vs_filename, const char* fs_filename);
	bgfx::ProgramHandle program;
};