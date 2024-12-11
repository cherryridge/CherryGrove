#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <cstdio>
#include <iostream>

#include "ShaderFactory.hpp"

using std::string;

static bgfx::ShaderHandle loadShader(const char* fileName) {
	string filePath = "";
	switch (bgfx::getRendererType()) {
		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12:
			filePath = "shaders/dx11/";
			break;
		case bgfx::RendererType::Metal:
			filePath = "shaders/metal/";
			break;
		case bgfx::RendererType::OpenGL:
			filePath = "shaders/glsl/";
			break;
		case bgfx::RendererType::Vulkan:
			filePath = "shaders/spirv/";
			break;
		case bgfx::RendererType::Noop:
		default:
			std::cout << "No valid render backends, exit!" << std::endl;
			std::exit(1);
			break;
	}
	filePath += fileName;
	FILE* file = fopen(filePath.c_str(), "rb");
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	const bgfx::Memory* mem = bgfx::alloc(fileSize + 1);
	fread(mem->data, 1, fileSize, file);
	mem->data[mem->size - 1] = '\0';
	fclose(file);
	return bgfx::createShader(mem);
}

ShaderFactory::ShaderFactory(const char* vsFileName, const char* fsFileName) {
	bgfx::ShaderHandle vsh = loadShader(vsFileName);
	bgfx::ShaderHandle fsh = loadShader(fsFileName);
	program = bgfx::createProgram(vsh, fsh, true);
}