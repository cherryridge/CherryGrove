#include <iostream>
#include <bgfx/bgfx.h>
#include <stb/stb_image.h>

#include "TextureFactory.hpp"

TextureFactory::TextureFactory(const char* filePath, bool noVerticalFilp) {
	if (!noVerticalFilp) stbi_set_flip_vertically_on_load(1);
	raw = stbi_load(filePath, &height, &width, &bitsPerPixel, 4);
	std::cout << height << " " << width << " " << bitsPerPixel << std::endl;
	handle = bgfx::createTexture2D(uint16_t(width), uint16_t(height), false, 1, bgfx::TextureFormat::RGBA8, BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT, bgfx::makeRef(raw, height * width * bitsPerPixel));
}

TextureFactory::~TextureFactory() {
	bgfx::destroy(handle);
	stbi_image_free(raw);
}