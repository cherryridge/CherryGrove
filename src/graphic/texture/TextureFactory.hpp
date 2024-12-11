#pragma once
#include <iostream>
#include <bgfx/bgfx.h>

class TextureFactory {
public:
	TextureFactory(const char* filePath, bool noVerticalFilp = false);
	~TextureFactory();

	bgfx::TextureHandle handle;
	int width;
	int height;
	int bitsPerPixel;
	unsigned char* raw;
};