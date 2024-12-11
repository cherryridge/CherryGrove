#include <bgfx/bgfx.h>

#include "TexturePool.hpp"
#include "TextureFactory.hpp"

typedef uint32_t u32;
typedef uint8_t u8;

TexturePool::TexturePool(const char* samplerName) {
	sampler = bgfx::createUniform(samplerName, bgfx::UniformType::Sampler);
}

TexturePool::~TexturePool() {
	for (u32 i = 0; i < registry.size(); i++) delete registry[i];
	bgfx::destroy(sampler);
}

u32 TexturePool::addTexture(const char* filePath, bool noVerticalFilp) {
	TextureFactory* texture = new TextureFactory(filePath, noVerticalFilp);
	registry.push_back(texture);
	return registry.size() - 1;
}

//Remember this :)
void TexturePool::useTexture(u32 id, u8 textureDataIndexForShaderToGetTextureCoordinates_aka__stage) { bgfx::setTexture(textureDataIndexForShaderToGetTextureCoordinates_aka__stage, sampler, registry[id]->handle); }

void TexturePool::removeTexture(u32 id) { delete registry[id]; }