#pragma once
#include <bgfx/bgfx.h>

namespace ShaderPool {
	typedef uint8_t u8;
	typedef uint16_t u16;
	typedef u16 ShaderID;

	inline constexpr u8 ERROR_SHADER_NOT_EXIST = 1u;

	void init();
	void shutdown();

	ShaderID addShader(const char* vsFileName, const char* fsFileName);
	const bgfx::ProgramHandle& getShader(ShaderID shaderId);
	void removeShader(ShaderID id);
}