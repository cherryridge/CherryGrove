#include <bgfx/bgfx.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <filesystem>

#include "ShaderPool.hpp"

typedef uint32_t u32;
typedef uint16_t u16;

using std::unordered_map, std::unique_ptr, std::string, std::cout, std::endl, std::make_unique, std::move, std::ifstream, std::filesystem::file_size, std::filesystem::exists, std::exit, bgfx::RendererType, bgfx::ShaderHandle, bgfx::ProgramHandle, bgfx::destroy;

namespace ShaderPool {
	u32 nextId;
	unordered_map<u32, unique_ptr<ProgramHandle>> registry;
	ShaderHandle loadShader(const char* fileName);

	void init() { nextId = 0; }

	void shutdown() { for (u32 i = 0; i < registry.size(); i++) destroy(*registry[i].get()); }

	u32 addShader(const char* vsFileName, const char* fsFileName) {
		ShaderHandle vsh = loadShader(vsFileName);
		ShaderHandle fsh = loadShader(fsFileName);
		ProgramHandle program = bgfx::createProgram(vsh, fsh, true);
		registry.emplace(nextId, make_unique<ProgramHandle>(move(program)));
		nextId++;
		return nextId - 1;
	}

	ProgramHandle getShader(u32 shaderId) {
		auto p = registry.find(shaderId);
		if (p == registry.end()) throw ERROR_SHADER_NOT_EXIST;
		return *(p->second.get());
	}

	void removeShader(u32 id) {
		auto p = registry.find(id);
		if (p == registry.end()) throw ERROR_SHADER_NOT_EXIST;
		destroy(*(p->second));
		registry.erase(p);
	}

	static ShaderHandle loadShader(const char* fileName) {
		string filePath = "";
		//todo: Use C++ exceptions on ALL of those `exit(1)`!
		switch (bgfx::getRendererType()) {
		case RendererType::Direct3D11:
		case RendererType::Direct3D12:
			filePath = "shaders/dx11/";
			break;
		case RendererType::Metal:
			filePath = "shaders/metal/";
			break;
		case RendererType::OpenGL:
			filePath = "shaders/glsl/";
			break;
		case RendererType::Vulkan:
			filePath = "shaders/spirv/";
			break;
		default:
			cout << "No valid render backends, exit!" << endl;
			exit(1);
			break;
		}
		filePath += fileName;
		if (!exists(filePath)) {
			cout << "Error: Shader file not found: " << filePath << endl;
			exit(1);
		}
		auto size = file_size(filePath);
		ifstream file(filePath, std::ios::binary);
		if (!file) {
			cout << "Error: Failed to open shader file: " << filePath << endl;
			exit(1);
		}
		const bgfx::Memory* memory = bgfx::alloc((u32)size + 1);
		file.read(reinterpret_cast<char*>(memory->data), size);
		if (!file) {
			cout << "Error: Failed to read shader file: " << filePath << endl;
			exit(1);
		}
		memory->data[memory->size - 1] = '\0';
		return bgfx::createShader(memory);
	}

}