#pragma once
#include <atomic>
#include <array>
#include <imgui.h>

namespace Renderer {
    typedef uint8_t u8;

    extern std::atomic<bool> initialized;
    extern std::atomic<bool> sizeUpdateSignal;
    
    //Must be called after `MainWindow::initInputHandler`.
    void start();
    void waitShutdown();

    inline constexpr u8 guiViewId        = 1u;
    inline constexpr u8 gameViewId       = 0u;

    struct Vertex {
        float x; float y; float z;
        //Texture coordinates are normalized from `i16` during conversion.
        float u; float v;
    };

    //Debug only.
    struct d_ColoredVertex {
        float x; float y; float z; float r; float g; float b; float a;
    };

    struct NormalVertex {
        float x; float y; float z;
        //Texture coordinates are normalized from `i16` during conversion.
        float u; float v;
        float nx; float ny; float nz;
    };

    //(up, down, north, east, south, west)
    inline constexpr std::array<Vertex, 24> blockVerticesTemplate = {
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

    inline constexpr std::array<int16_t, 6> blockIndicesTemplate = { 0, 2, 1, 1, 2, 3 };
}