#pragma once
#include <bgfx/bgfx.h>
#include <optional>

namespace TexturePool {
	typedef uint8_t u8;
	typedef uint16_t u16;
	typedef uint32_t TextureID;

	struct Texture {
		bgfx::TextureHandle handle;
		u16 width;
		u16 height;
		u8 bitsPerPixel;
		unsigned char* raw;
	};

	void init(const char* samplerName);
	void shutdown();
	TextureID addTexture(const char* filePath, bool noVerticalFilp = false);
	void useTexture(TextureID id, u8 textureDataIndex = 0);
	std::optional<const Texture*> getTexture(TextureID id);
	void removeTexture(TextureID id);
}