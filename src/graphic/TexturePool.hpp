#pragma once
#include <bgfx/bgfx.h>

typedef uint8_t u8;
typedef uint32_t u32;

namespace TexturePool {
	constexpr u8 ERROR_TEXTURE_NOT_EXIST = 1u;

	struct Texture {
		bgfx::TextureHandle handle;
		int width;
		int height;
		int bitsPerPixel;
		unsigned char* raw;
	};

	void init(const char* samplerName);
	void shutdown();
	u32 addTexture(const char* filePath, bool noVerticalFilp = false);
	void useTexture(u32 id, u8 textureDataIndex = 0);
	void removeTexture(u32 id);
}