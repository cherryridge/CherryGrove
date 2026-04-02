#pragma once
#include <array>
#include <vector>

#include "../../graphics/Renderer.hpp"

namespace Components {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::array, std::vector, Renderer::Vertex;

    //Chunk size: 2^5 = 32, which will probably not change during the whole lifecycle of the game software itself, but we have to be extra aware, so we don't hardcode it anywhere else.
    inline constexpr u8 CHUNK_SIZE_MAGNITUDE = 5;

    struct ChunkMesh {
        vector<Vertex> vertices;
        vector<u32> indices;
    };

    struct ChunkBlockDataComp {
        array<u64, static_cast<u64>(CHUNK_SIZE_MAGNITUDE) * CHUNK_SIZE_MAGNITUDE * CHUNK_SIZE_MAGNITUDE> blockIds;

        ChunkMesh mesh() const noexcept {
            
        }
    };
}