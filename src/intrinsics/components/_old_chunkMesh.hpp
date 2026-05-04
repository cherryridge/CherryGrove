#pragma once
#include <array>
#include <cstdint>
#include <vector>

#include "../../graphics/Renderer.hpp"

namespace Components {
    typedef uint8_t u8;
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::array, std::vector, Renderer::Vertex;

    //Chunk size: 2^5 = 32, which will probably not change during the whole lifecycle of CherryGrove itself, but we have to be extra aware, so we don't hardcode it anywhere else.
    inline constexpr u8 CHUNK_SIZE_MAGNITUDE = 5;
    inline constexpr u64 CHUNK_SIZE = 1ull << CHUNK_SIZE_MAGNITUDE;
    inline constexpr u64 CHUNK_LAYER_BLOCK_COUNT = CHUNK_SIZE * CHUNK_SIZE;
    inline constexpr u64 CHUNK_BLOCK_COUNT = CHUNK_LAYER_BLOCK_COUNT * CHUNK_SIZE;

    inline constexpr array<array<i32, 3>, 6> CHUNK_FACE_NEIGHBOR_OFFSETS = {{
        {{ 0,  1,  0}},
        {{ 0, -1,  0}},
        {{ 0,  0, -1}},
        {{ 1,  0,  0}},
        {{ 0,  0,  1}},
        {{-1,  0,  0}},
    }};

    struct ChunkMesh {
        vector<Vertex> vertices;
        vector<u32> indices;
    };

    struct ChunkFaceUVRect {
        float u0 {0.0f};
        float v0 {0.0f};
        float u1 {1.0f};
        float v1 {1.0f};
    };

    struct ChunkMesherLookup {
        using ExternalBlockLookupFn = u64 (*)(i32 x, i32 y, i32 z, const void* userData) noexcept;
        using BlockPredicateFn = bool (*)(u64 blockId, const void* userData) noexcept;
        using FaceUVLookupFn = ChunkFaceUVRect (*)(u64 blockId, u8 faceIndex, const void* userData) noexcept;

        const void* userData {};
        //Receives local chunk coordinates. Out-of-range values indicate a neighboring chunk lookup.
        ExternalBlockLookupFn externalBlockLookup {};
        //Controls whether a block ID should emit geometry at all. Defaults to `blockId != 0`.
        BlockPredicateFn blockExists {};
        //Controls whether a neighboring block hides a face. Defaults to the same rule as `blockExists`.
        BlockPredicateFn blockOccludes {};
        //Returns normalized UV bounds for a specific block face. Defaults to the full [0, 1] range.
        FaceUVLookupFn faceUVLookup {};

        [[nodiscard]] bool hasBlock(u64 blockId) const noexcept {
            return blockExists ? blockExists(blockId, userData) : blockId != 0;
        }

        [[nodiscard]] bool occludes(u64 blockId) const noexcept {
            return blockOccludes ? blockOccludes(blockId, userData) : hasBlock(blockId);
        }

        [[nodiscard]] ChunkFaceUVRect faceUV(u64 blockId, u8 faceIndex) const noexcept {
            return faceUVLookup ? faceUVLookup(blockId, faceIndex, userData) : ChunkFaceUVRect {};
        }
    };

    struct ChunkBlockDataComp {
        array<u64, CHUNK_BLOCK_COUNT> blockIds {};

        [[nodiscard]] static constexpr bool contains(i32 x, i32 y, i32 z) noexcept {
            return x >= 0 && x < static_cast<i32>(CHUNK_SIZE) && y >= 0 && y < static_cast<i32>(CHUNK_SIZE) && z >= 0 && z < static_cast<i32>(CHUNK_SIZE);
        }

        [[nodiscard]] static constexpr u64 blockIndex(i32 x, i32 y, i32 z) noexcept {
            return static_cast<u64>(x) + static_cast<u64>(z) * CHUNK_SIZE + static_cast<u64>(y) * CHUNK_LAYER_BLOCK_COUNT;
        }

        [[nodiscard]] u64 blockIdAt(i32 x, i32 y, i32 z) const noexcept {
            return contains(x, y, z) ? blockIds[blockIndex(x, y, z)] : 0ull;
        }

        void setBlockIdAt(i32 x, i32 y, i32 z, u64 blockId) noexcept {
            if (contains(x, y, z)) blockIds[blockIndex(x, y, z)] = blockId;
        }

        [[nodiscard]] ChunkMesh mesh() const noexcept {
            return mesh(ChunkMesherLookup {});
        }

        [[nodiscard]] ChunkMesh mesh(const ChunkMesherLookup& lookup) const noexcept {
            ChunkMesh result;

            for (i32 y = 0; y < static_cast<i32>(CHUNK_SIZE); y++) {
                for (i32 z = 0; z < static_cast<i32>(CHUNK_SIZE); z++) {
                    for (i32 x = 0; x < static_cast<i32>(CHUNK_SIZE); x++) {
                        const u64 currentBlockId = blockIds[blockIndex(x, y, z)];
                        if (!lookup.hasBlock(currentBlockId)) continue;

                        for (u8 faceIndex = 0; faceIndex < CHUNK_FACE_NEIGHBOR_OFFSETS.size(); faceIndex++) {
                            const auto& neighborOffset = CHUNK_FACE_NEIGHBOR_OFFSETS[faceIndex];
                            const i32 neighborX = x + neighborOffset[0];
                            const i32 neighborY = y + neighborOffset[1];
                            const i32 neighborZ = z + neighborOffset[2];
                            const u64 neighborBlockId = contains(neighborX, neighborY, neighborZ)
                                ? blockIds[blockIndex(neighborX, neighborY, neighborZ)]
                                : (lookup.externalBlockLookup ? lookup.externalBlockLookup(neighborX, neighborY, neighborZ, lookup.userData) : 0ull);
                            if (lookup.occludes(neighborBlockId)) continue;

                            const ChunkFaceUVRect uv = lookup.faceUV(currentBlockId, faceIndex);
                            const float du = uv.u1 - uv.u0;
                            const float dv = uv.v1 - uv.v0;
                            const u32 vertexBaseIndex = static_cast<u32>(result.vertices.size());
                            const u32 templateOffset = static_cast<u32>(faceIndex) * 4u;

                            for (u32 vertexOffset = 0; vertexOffset < 4u; vertexOffset++) {
                                const Vertex& baseVertex = Renderer::blockVerticesTemplate[templateOffset + vertexOffset];
                                result.vertices.emplace_back(Vertex {
                                    baseVertex.x + static_cast<float>(x),
                                    baseVertex.y + static_cast<float>(y),
                                    baseVertex.z + static_cast<float>(z),
                                    uv.u0 + baseVertex.u * du,
                                    uv.v0 + baseVertex.v * dv,
                                });
                            }

                            for (const auto index : Renderer::blockIndicesTemplate) result.indices.push_back(vertexBaseIndex + static_cast<u32>(index));
                        }
                    }
                }
            }

            return result;
        }
    };
}