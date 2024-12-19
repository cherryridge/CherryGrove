#pragma once
#include <bgfx/bgfx.h>

typedef uint8_t u8;
typedef uint32_t u32;

namespace ShaderPool {
	constexpr u8 ERROR_SHADER_NOT_EXIST = 1u;

	void init();
	void shutdown();

	u32 addShader(const char* vsFileName, const char* fsFileName);
	bgfx::ProgramHandle getShader(u32 shaderId);
	void removeShader(u32 id);
}