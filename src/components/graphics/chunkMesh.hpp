#pragma once
#include <array>
#include <vector>

namespace Components {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::array, std::vector;

    //Chunk size: 2^5 = 32
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