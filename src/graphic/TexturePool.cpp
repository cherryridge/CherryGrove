#include <bgfx/bgfx.h>
#include <stb/stb_image.h>
#include <optional>
#include <unordered_map>
#include <limits>
#include <memory>

#include "../debug/debug.hpp"
#include "TexturePool.hpp"

//MASSIVE todo: store texture in one texture atlas and use textureId to index coordinates.
//as well as dynamically managing texture size, allowing registering 32x32 or bigger texture.
namespace TexturePool {
	typedef uint8_t u8;
	typedef int16_t i16;
	typedef uint32_t u32;
	typedef u32 TextureID;

	using std::unordered_map, bgfx::UniformHandle, bgfx::createUniform, bgfx::destroy, std::optional, std::move, std::numeric_limits;

	TextureID addTexture(const char* filePath, bool noVerticalFilp);

	unordered_map<TextureID, Texture> registry;
	TextureID nextId;
	UniformHandle sampler;

	void init(const char* samplerName) {
		sampler = createUniform(samplerName, bgfx::UniformType::Sampler);
		nextId = 0;
		addTexture("assets/textures/missing.png");
	}

	void shutdown() {
		destroy(sampler);
		for (const auto& pair : registry) {
			destroy(pair.second.handle);
			stbi_image_free(pair.second.raw);
		}
	}

	TextureID addTexture(const char* filePath, bool noVerticalFilp) {
		Texture texture{};
		if (!noVerticalFilp) stbi_set_flip_vertically_on_load(1);
		else stbi_set_flip_vertically_on_load(0);
		int _height, _width, _bpp;
		texture.raw = stbi_load(filePath, &_height, &_width, &_bpp, 4);
		if (_height > numeric_limits<i16>::max()) {
			lerr << "Texture height overflow (highest " << numeric_limits<i16>::max() << "): " << filePath << endl;
			return 0;
		}
		if (_width > numeric_limits<i16>::max()) {
			lerr << "Texture width overflow (highest " << numeric_limits<i16>::max() << "): " << filePath << endl;
			return 0;
		}
		texture.height = _height;
		texture.width = _width;
		texture.bitsPerPixel = _bpp;
		texture.handle = bgfx::createTexture2D(
			texture.width,
			texture.height,
			false, 1,
			//Temporary!
			_bpp == 3 ? bgfx::TextureFormat::RGB8 : bgfx::TextureFormat::RGBA8,
			BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT,
			bgfx::makeRef(texture.raw, texture.height * texture.width * texture.bitsPerPixel)
		);
		registry.emplace(nextId, move(texture));
		nextId++;
		return nextId - 1;
	}

	void useTexture(TextureID id, u8 textureDataIndex) {
		auto p = registry.find(id);
		//Fallback to a missing texture. See `init()`.
		if (p == registry.end()) bgfx::setTexture(textureDataIndex, sampler, registry[0].handle);
		bgfx::setTexture(textureDataIndex, sampler, p->second.handle);
	}

	optional<const Texture*> getTexture(TextureID id) {
		auto p = registry.find(id);
		if (p == registry.end()) return std::nullopt;
		return &(p->second);
	}

	void removeTexture(TextureID id) {
		auto p = registry.find(id);
		if (p == registry.end()) return;
		destroy(p->second.handle);
		stbi_image_free(p->second.raw);
		registry.erase(p);
	}
}