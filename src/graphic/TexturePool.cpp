#include <bgfx/bgfx.h>
#include <stb/stb_image.h>
#include <unordered_map>
#include <memory>

#include "TexturePool.hpp"

typedef uint8_t u8;
typedef uint32_t u32;

using std::unordered_map, bgfx::UniformHandle, bgfx::createUniform, bgfx::destroy, std::make_unique, std::move, std::unique_ptr;

//MASSIVE todo: store texture in one texture atlas and use textureId to index coordinates.
//as well as dynamically managing texture size, allowing registering 32x32 or bigger texture.
namespace TexturePool {
	unordered_map<u32, unique_ptr<Texture>> registry;
	u32 nextId;
	UniformHandle sampler;

	void init(const char* samplerName) {
		sampler = createUniform(samplerName, bgfx::UniformType::Sampler);
		nextId = 0;
	}

	void shutdown() {
		destroy(sampler);
		for (u32 i = 0; i < registry.size(); i++) {
			destroy(registry[i]->handle);
			stbi_image_free(registry[i]->raw);
		}
	}

	u32 addTexture(const char* filePath, bool noVerticalFilp) {
		auto texture = make_unique<Texture>();
		if (!noVerticalFilp) stbi_set_flip_vertically_on_load(1);
		else stbi_set_flip_vertically_on_load(0);
		texture->raw = stbi_load(filePath, &texture->height, &texture->width, &texture->bitsPerPixel, 4);
		texture->handle = bgfx::createTexture2D(
			uint16_t(texture->width),
			uint16_t(texture->height),
			false, 1,
			bgfx::TextureFormat::RGBA8,
			BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT,
			bgfx::makeRef(texture->raw, texture->height * texture->width * texture->bitsPerPixel)
		);
		registry.emplace(nextId, move(texture));
		nextId++;
		return nextId - 1;
	}

	void useTexture(u32 id, u8 textureDataIndex) {
		auto p = registry.find(id);
		if (p == registry.end()) throw ERROR_TEXTURE_NOT_EXIST;
		bgfx::setTexture(textureDataIndex, sampler, p->second->handle);
	}

	void removeTexture(u32 id) {
		auto p = registry.find(id);
		if (p == registry.end()) throw ERROR_TEXTURE_NOT_EXIST;
		Texture texture = *(p->second);
		destroy(texture.handle);
		stbi_image_free(texture.raw);
		registry.erase(p);
	}
}