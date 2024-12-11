#include <bgfx/bgfx.h>
#include <vector>

#include "ShaderPool.hpp"
#include "ShaderFactory.hpp"

typedef uint32_t u32;
typedef uint16_t u16;

using std::vector;

ShaderPool::ShaderPool() {
	
}

ShaderPool::~ShaderPool() {
	for (u32 i = 0; i < registry.size(); i++) delete registry[i];
}

u32 ShaderPool::addShader(const char* vsFileName, const char* fsFileName) {
	ShaderFactory* shader = new ShaderFactory(vsFileName, fsFileName);
	registry.push_back(shader);
	return registry.size() - 1;
}

void ShaderPool::useShader(u32 id, u16 viewId) { bgfx::submit(viewId, registry[id]->program); }

void ShaderPool::removeShader(u32 id) { delete registry[id]; }