#pragma once
#include <bgfx/bgfx.h>
#include <vector>

#include "ShaderFactory.hpp"

typedef uint32_t u32;
typedef uint16_t u16;

using std::vector;

//@singleton
class ShaderPool {
public:
	ShaderPool();
	~ShaderPool();

	u32 addShader(const char* vsFileName, const char* fsFileName);
	void removeShader(u32 id);
	void useShader(u32 id, u16 viewId);

private:
	vector<ShaderFactory*> registry;
};