#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <cstdio>
#include <iostream>

#include "shader.hpp"

using std::string;

static bgfx::ShaderHandle loadShader(const char* FILENAME) {
	const char* shaderPath = "";
	switch (bgfx::getRendererType()) {
		case bgfx::RendererType::Noop:
		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12:
			shaderPath = "shaders/dx11/";
			break;
		case bgfx::RendererType::Metal:
			shaderPath = "shaders/metal/";
			break;
		case bgfx::RendererType::OpenGL:
			shaderPath = "shaders/glsl/";
			break;
		case bgfx::RendererType::Vulkan:
			shaderPath = "shaders/spirv/";
			break;
	}
	size_t shaderLen = strlen(shaderPath);
	size_t fileLen = strlen(FILENAME);
	char* filePath = (char*)calloc(1, shaderLen + fileLen + 1);
	memcpy(filePath, shaderPath, shaderLen);
	memcpy(&filePath[shaderLen], FILENAME, fileLen);
	FILE* file = fopen(filePath, "rb");
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	const bgfx::Memory* mem = bgfx::alloc(fileSize + 1);
	fread(mem->data, 1, fileSize, file);
	mem->data[mem->size - 1] = '\0';
	fclose(file);
	return bgfx::createShader(mem);
}

Shader::Shader(const char* vs_filename, const char* fs_filename) {
	bgfx::ShaderHandle vsh = loadShader(vs_filename);
	bgfx::ShaderHandle fsh = loadShader(fs_filename);
	program = bgfx::createProgram(vsh, fsh, true);
}