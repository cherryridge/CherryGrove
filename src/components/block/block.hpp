#pragma once
#include <array>
#include <cstdint>
#include <limits>
#include <glm/glm.hpp>
#include <boost/unordered/unordered_flat_map.hpp>

#include "../../debug/Logger.hpp"
#include "../../graphic/TexturePool.hpp"

namespace Components {
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef int64_t i64;
    typedef uint64_t u64;
    using std::array, boost::unordered::unordered_flat_map, std::numeric_limits, glm::uvec2, glm::vec3, glm::vec4, TexturePool::getTextureInfo, TexturePool::TextureInfo;

    struct BlockCoordinatesComp {
        i64 x;
        i64 y;
        i64 z;
        u64 dimensionId;
    };

    struct CubeFace {
        //{U1, V1, U2, V2}
        //U2 >= U1 && V2 >= V1
        vec4 texCoords;
        float uvRotation;
        u32 shaderId;
        u32 textureId;

        CubeFace() noexcept = default;
        CubeFace(const uvec2& rectStartCoords, const uvec2& rectEndCoords, float uvRotation, u32 shaderId, u32 textureId) noexcept : uvRotation(uvRotation), shaderId(shaderId), textureId(textureId) {
            TextureInfo info;
            if (!getTextureInfo(textureId, info)) {
                lerr << "[CubeFace] Texture not registered!" << endl;
                return;
            }
            if (rectStartCoords.x < rectEndCoords.x || rectStartCoords.y < rectEndCoords.y) lerr << "[CubeFace] Texture coordinates malformed." << endl;
            texCoords.x = static_cast<float>(rectStartCoords.x) / info.width;
            texCoords.y = static_cast<float>(rectStartCoords.y) / info.height;
            texCoords.z = static_cast<float>(rectEndCoords.x) / info.width;
            texCoords.w = static_cast<float>(rectEndCoords.y) / info.height;
        }
    };

    struct SubCube {
        //All coordinates are relative to block coordinates (corner with least coordinates).
        vec3 origin;
        vec3 size;
        //Selection box origin
        vec3 sOrigin;
        //Selection box size, if no selection box should be displayed, set to (0.0f).
        vec3 sSize;
        vec3 pivot;
        //(rotationX, rotationY, rotationZ), if no rotation, set to (0.0f).
        vec3 rotation;
        //(up, down, north, east, south, west)
        array<CubeFace, 6> faces;

        SubCube() noexcept = default;
        SubCube(vec3 origin, vec3 size, vec3 sOrigin, vec3 sSize, vec3 pivot, vec3 rotation, CubeFace faceUp, CubeFace faceDown, CubeFace faceNorth, CubeFace faceEast, CubeFace faceSouth, CubeFace faceWest) noexcept : origin(origin), size(size), sOrigin(sOrigin), sSize(sSize), pivot(pivot), rotation(rotation), faces({faceUp, faceDown, faceNorth, faceEast, faceSouth, faceWest}) {}
    };

    struct BlockRenderComp {
        unordered_flat_map<u32, SubCube> subcubes;

        BlockRenderComp() noexcept = default;
        //Single cube convention
        BlockRenderComp(SubCube sc) noexcept : subcubes({{0u, sc}}) {}
    };
}