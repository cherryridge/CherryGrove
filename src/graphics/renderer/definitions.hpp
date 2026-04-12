#pragma once
#include <cstdint>

namespace Renderer {
    typedef uint8_t u8;

    inline constexpr u8 VIEWID_GAME = 0, VIEWID_STARTING_GUI = 1, VIEWIDS_GUI = 16;

    struct Vertex {
        //World coordinates in block units.
        float x, y, z;
        //Texture coordinates are normalized from `u16` during rendering because bgfx doesn't support `i16` integer attributes, and using `float` directly is more efficient than converting to `float` in the shader.
        float u, v;
    };
}