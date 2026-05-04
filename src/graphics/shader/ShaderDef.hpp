#pragma once
#include <cstdint>

namespace ShaderPool {
    typedef uint8_t u8;
    typedef uint64_t u64;

    struct ShaderDef {
        const u8* dx11;
        u64 dx11_size;
        const u8* dx12;
        u64 dx12_size;
        const u8* metal;
        u64 metal_size;
        const u8* opengl;
        u64 opengl_size;
        const u8* opengles;
        u64 opengles_size;
        const u8* vulkan;
        u64 vulkan_size;
    };

    struct ShaderSetDef {
        ShaderDef vs;
        ShaderDef fs;
    };
}