#pragma once
#include <vector>
#include <bgfx/bgfx.h>

#include "TextureFactory.hpp"

typedef uint32_t u32;
typedef uint8_t u8;

using std::vector;

//@singleton
//multithread rendering?
class TexturePool {
public:
	TexturePool(const char* samplerName);
	~TexturePool();

	u32 addTexture(const char* filePath, bool noVerticalFilp = false);
	void removeTexture(u32 id);
	void useTexture(u32 id, u8 textureDataIndexForShaderToGetTextureCoordinates_aka__stage = 0);

private:
	vector<TextureFactory*> registry;
	bgfx::UniformHandle sampler;
};