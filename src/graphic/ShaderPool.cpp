#include <bgfx/bgfx.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include "../debug/debug.hpp"
#include "ShaderPool.hpp"

namespace ShaderPool {
	typedef uint32_t u32;
	typedef uint16_t u16;
	typedef u16 ShaderID;

	using std::unordered_map, std::string, std::ifstream, std::filesystem::file_size, std::filesystem::exists, bgfx::RendererType, bgfx::ShaderHandle, bgfx::ProgramHandle, bgfx::destroy;

	static ShaderID nextId;
	static unordered_map<ShaderID, ProgramHandle> registry;
	static ShaderHandle loadShader(const char* fileName);

	void init() { nextId = 0; }

	void shutdown() { for (const auto& shader : registry) destroy(shader.second); }

	ShaderID addShader(const char* vsFileName, const char* fsFileName) {
		auto vsh = loadShader(vsFileName), fsh = loadShader(fsFileName);
		registry.emplace(nextId, bgfx::createProgram(vsh, fsh, true));
		nextId++;
		return nextId - 1;
	}

	const ProgramHandle& getShader(ShaderID shaderId) {
		auto p = registry.find(shaderId);
		if (p == registry.end()) {
			lerr << "[ShaderPool] Failed to get shader " << shaderId << "!" << endl;
			Fatal::exit(Fatal::BGFX_GET_SHADER_FAILED);
		}
		return p->second;
	}

	void removeShader(ShaderID id) {
		auto p = registry.find(id);
		if (p == registry.end()) return;
		destroy(p->second);
		registry.erase(p);
	}

	static ShaderHandle loadShader(const char* fileName) {
		string filePath = "shaders/";
		switch (bgfx::getRendererType()) {
		case RendererType::Direct3D11:
		case RendererType::Direct3D12:
			filePath += "dx11/";
			break;
		case RendererType::Metal:
			filePath += "metal/";
			break;
		case RendererType::OpenGL:
			filePath += "glsl/";
			break;
		case RendererType::Vulkan:
			filePath += "spirv/";
			break;
		default:
			lerr << "[ShaderPool] No valid render backends!" << endl;
			Fatal::exit(Fatal::BGFX_NO_VALID_RENDER_BACKEND);
			break;
		}
		filePath += fileName;
		if (!exists(filePath)) {
			lerr << "[ShaderPool] Shader file not found: " << filePath << endl;
			Fatal::exit(Fatal::BGFX_SHADER_FILE_NOT_FOUND);
		}
		auto size = file_size(filePath);
		ifstream file(filePath, std::ios::binary);
		if (!file) {
			lerr << "[ShaderPool] Failed to open shader file: " << filePath << endl;
			Fatal::exit(Fatal::BGFX_OPEN_SHADER_FILE_FAILED);
		}
		const bgfx::Memory* memory = bgfx::alloc((u32)size + 1);
		file.read(reinterpret_cast<char*>(memory->data), size);
		if (!file) {
			lerr << "[ShaderPool] Failed to read shader file: " << filePath << endl;
			Fatal::exit(Fatal::BGFX_READ_SHADER_FILE_FAILED);
		}
		memory->data[memory->size - 1] = '\0';
		return bgfx::createShader(memory);
	}
}