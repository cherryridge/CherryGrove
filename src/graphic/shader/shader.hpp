#pragma once
#include <iostream>
#include <bgfx/bgfx.h>

using std::string;

class Shader{
public:
	Shader(const string& vs_filename, const string& fs_filename);
	bgfx::ProgramHandle program;
};