#pragma once
#include <cstdint>
#include <glm/glm.hpp>

namespace Components {
    typedef int64_t i64;
    typedef uint64_t u64;

    struct EntityCoordinates {
        double x{0.0}, y{0.0}, z{0.0};
        u64 dimension{0};

        [[nodiscard]] bool operator==(const EntityCoordinates& other) const noexcept {
            return x == other.x && y == other.y && z == other.z && dimension == other.dimension;
        }

        [[nodiscard]] glm::vec3 getGLMVec3() const noexcept {
            return glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
        }
    };

    struct EntityCoordinates2 {
        i64 x{0}, y{0}, z{0};
        u64 dimension{0};
        float subX{0.0f}, subY{0.0f}, subZ{0.0f};

        [[nodiscard]] glm::vec<3, double> getRelativePos(const EntityCoordinates2& other) const noexcept {
            return glm::vec<3, double>(
                static_cast<double>(x - other.x) + static_cast<double>(subX - other.subX),
                static_cast<double>(y - other.y) + static_cast<double>(subY - other.subY),
                static_cast<double>(z - other.z) + static_cast<double>(subZ - other.subZ)
            );
        }

        [[nodiscard]] bool operator==(const EntityCoordinates2& other) const noexcept {
            return x == other.x && y == other.y && z == other.z && dimension == other.dimension && subX == other.subX && subY == other.subY && subZ == other.subZ;
        }
    };

    struct BlockCoordinates {
        i64 x{0}, y{0}, z{0};
        u64 dimension{0};

        [[nodiscard]] bool operator==(const BlockCoordinates& other) const noexcept {
            return x == other.x && y == other.y && z == other.z;
        }
    };

    struct ChunkCoordinates {
        i64 x{0}, y{0}, z{0};
        u64 dimension{0};

        [[nodiscard]] bool operator==(const ChunkCoordinates& other) const noexcept {
            return x == other.x && y == other.y && z == other.z;
        }
    };
}