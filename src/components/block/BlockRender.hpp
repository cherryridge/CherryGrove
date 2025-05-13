#pragma once
#include <unordered_map>
#include <array>
#include <cstdint>
#include <limits>
#include <glm/glm.hpp>

#include "../../debug/Logger.hpp"
#include "../../graphic/TexturePool.hpp"

namespace Components {
    struct CubeFace {
        //{U, V, deltaU, deltaV}
        //glm::i16vec4 texCoords;
        //{U1, V1, U2, V2}
        //U2 >= U1 && V2 >= V1
        glm::vec4 texCoords;
        float uvRotation;
        uint32_t shaderId;
        uint32_t textureId;

        CubeFace() = default;
        CubeFace(glm::uvec2 tStart, glm::uvec2 tEnd, float uvRotation, uint32_t shaderId, uint32_t textureId) {
            int32_t dx = tEnd.x - tStart.x, dy = tEnd.y - tStart.y;
            auto _texture = TexturePool::getTexture(textureId);
            if (!_texture.has_value()) {
                lerr << "[CubeFace] Texture not registered!" << endl;
                return;
            }
            auto texture = _texture.value();
            if (tStart.x > texture->width || tEnd.x > texture->width || tStart.y > texture->height || tEnd.y > texture->height) lerr << "[CubeFace] Texture coordinates overflow in texture dimensions." << endl;
            //todo: Cap the value instead of blowing up :)
            if (tStart.x > std::numeric_limits<int16_t>::max() || tStart.y > std::numeric_limits<int16_t>::max() || dx > std::numeric_limits<int16_t>::max() || dy > std::numeric_limits<int16_t>::max() || dx < std::numeric_limits<int16_t>::min() || dy < std::numeric_limits<int16_t>::min()) lerr << "[CubeFace] Texture coordinates overflow in short type." << endl;
            texCoords.x = (float)tStart.x / texture->width;
            texCoords.y = (float)tStart.y / texture->height;
            texCoords.z = (float)dx;
            texCoords.w = (float)dy;
            this->uvRotation = uvRotation;
            this->shaderId = shaderId;
            this->textureId = textureId;
        }
    };

    struct SubCube {
        //All coordinates are relative to block coordinates (corner with least coordinates).
        glm::vec3 origin;
        glm::vec3 size;
        //Selection box origin
        glm::vec3 sOrigin;
        //Selection box size, if no selection box should be displayed, set to (0.0f).
        glm::vec3 sSize;
        glm::vec3 pivot;
        //(rotationX, rotationY, rotationZ), if no rotation, set to (0.0f).
        glm::vec3 rotation;
        //(up, down, north, east, south, west)
        std::array<CubeFace, 6> faces;
        

        SubCube() = default;
        SubCube(glm::vec3 origin, glm::vec3 size, glm::vec3 sOrigin, glm::vec3 sSize, glm::vec3 pivot, glm::vec3 rotation, CubeFace faceUp, CubeFace faceDown, CubeFace faceNorth, CubeFace faceEast, CubeFace faceSouth, CubeFace faceWest) {
            this->origin = origin;
            this->size = size;
            this->sOrigin = sOrigin;
            this->sSize = sSize;
            this->pivot = pivot;
            this->rotation = rotation;
            this->faces = std::array<CubeFace, 6>{ faceUp, faceDown, faceNorth, faceEast, faceSouth, faceWest };
        }
    };

    struct BlockRenderComponent {
        std::unordered_map<uint32_t, SubCube> subcubes;

        BlockRenderComponent() = default;
        //Single cube convention
        BlockRenderComponent(SubCube sc) {
            subcubes.emplace(0u, sc);
        }
    };
}