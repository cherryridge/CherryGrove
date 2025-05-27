#pragma once
#include <atomic>
#include <array>

namespace Renderer {
    typedef uint8_t u8;
    typedef int32_t i32;
    using std::atomic, std::array;

    extern atomic<bool> initialized;
    extern atomic<bool> sizeUpdateSignal;
    
    //Must be called after `MainWindow::initInputHandler`.
    void start() noexcept;
    void shutdown() noexcept;

    inline constexpr u8 guiViewId        = 1u;
    inline constexpr u8 gameViewId       = 0u;

    struct WindowInfoCache {
        i32 width;
        i32 height;
        float aspectRatio;
    };

    extern WindowInfoCache cache;

    struct Vertex {
        float x, y, z;
        //Texture coordinates are normalized from `i16` during conversion.
        float u, v;
    };

    //Debug only.
    struct d_ColoredVertex {
        float x, y, z, r, g, b, a;
    };

    struct NormalVertex {
        float x, y, z;
        //Texture coordinates are normalized from `i16` during conversion.
        float u, v;
        float nx, ny, nz;
    };

    //(up, down, north, east, south, west)
    inline constexpr array<Vertex, 24> blockVerticesTemplate = {
        //Up
        Vertex{ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        Vertex{ 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        Vertex{ 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
        Vertex{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
        //Down
        Vertex{ 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
        Vertex{ 1.0f, 0.0f, 1.0f, 1.0f, 0.0f },
        Vertex{ 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
        Vertex{ 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
        //North (-Z)
        Vertex{ 1.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        Vertex{ 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        Vertex{ 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
        Vertex{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
        //East (+X)
        Vertex{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
        Vertex{ 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
        Vertex{ 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
        Vertex{ 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
        //South (+Z)
        Vertex{ 0.0f, 1.0f, 1.0f, 0.0f, 0.0f },
        Vertex{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
        Vertex{ 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
        Vertex{ 1.0f, 0.0f, 1.0f, 1.0f, 1.0f },
        //West (-X)
        Vertex{ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        Vertex{ 0.0f, 1.0f, 1.0f, 1.0f, 0.0f },
        Vertex{ 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
        Vertex{ 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
    };

    inline constexpr array<int16_t, 6> blockIndicesTemplate = { 0, 2, 1, 1, 2, 3 };
}