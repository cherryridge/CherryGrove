#pragma once
#include <cstdint>

namespace Simulation {
    typedef uint8_t u8;
    typedef uint64_t u64;

    //Chunk size: 2^5 = 32, which will probably not change during the whole lifecycle of CherryGrove itself, but we have to be extra aware, so we don't hardcode it anywhere else.
    inline constexpr u8 CHUNK_SIZE_MAGNITUDE = 5;
    inline constexpr u64
        CHUNK_SIZE = 1ull << CHUNK_SIZE_MAGNITUDE,
        CHUNK_LAYER_BLOCK_COUNT = CHUNK_SIZE * CHUNK_SIZE,
        CHUNK_BLOCK_COUNT = CHUNK_LAYER_BLOCK_COUNT * CHUNK_SIZE;

    struct Chunk {
        vector<BlockID>
    };
}