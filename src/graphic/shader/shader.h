#pragma once
#include <bgfx/bgfx.h>

class Shader{
public:
	Shader(const char* vs_filename, const char* fs_filename);
	bgfx::ProgramHandle program;
private:
	bgfx::ShaderHandle loadShader(const char* FILENAME);
};