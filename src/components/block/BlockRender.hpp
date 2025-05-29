#pragma once
#include <array>
#include <cstdint>
#include <limits>
#include <glm/glm.hpp>
#include <unordered_map>

#include "../../debug/Logger.hpp"
#include "../../graphic/TexturePool.hpp"

namespace Components {
    typedef uint16_t u16;
    typedef uint32_t u32;
    using std::array, std::unordered_map, std::numeric_limits, glm::uvec2, glm::vec3, glm::vec4;

    struct CubeFace {
        //{U1, V1, U2, V2}
        //U2 >= U1 && V2 >= V1
        vec4 texCoords;
        float uvRotation;
        u32 shaderId;
        u32 textureId;

        CubeFace() noexcept = default;
        CubeFace(const uvec2& rectStartCoords, const uvec2& rectEndCoords, float uvRotation, u32 shaderId, u32 textureId) noexcept : uvRotation(uvRotation), shaderId(shaderId), textureId(textureId) {
            const auto* texture = TexturePool::getTexture(textureId);
            if (texture == nullptr) {
                lerr << "[CubeFace] Texture not registered!" << endl;
                return;
            }
            if (rectStartCoords.x < rectEndCoords.x || rectStartCoords.y < rectEndCoords.y) lerr << "[CubeFace] Texture coordinates malformed." << endl;
            texCoords.x = (float)rectStartCoords.x / texture->data->w;
            texCoords.y = (float)rectStartCoords.y / texture->data->h;
            texCoords.z = (float)rectEndCoords.x / texture->data->w;
            texCoords.w = (float)rectEndCoords.y / texture->data->h;
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

    struct BlockRenderComponent {
        unordered_map<u32, SubCube> subcubes;

        BlockRenderComponent() noexcept = default;
        //Single cube convention
        BlockRenderComponent(SubCube sc) noexcept : subcubes({{0u, sc}}) {}
    };
}