#pragma once
#include <bgfx/bgfx.h>

namespace TexturePool {
	typedef uint8_t u8;
	typedef uint32_t TextureID;

	struct Texture {
		bgfx::TextureHandle handle;
		int width;
		int height;
		int bitsPerPixel;
		unsigned char* raw;
	};

	void init(const char* samplerName);
	void shutdown();
	TextureID addTexture(const char* filePath, bool noVerticalFilp = false);
	void useTexture(TextureID id, u8 textureDataIndex = 0);
	void removeTexture(TextureID id);
}