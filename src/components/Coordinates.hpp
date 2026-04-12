#pragma once
#include <cstdint>

namespace Components {
    typedef int64_t i64;
    typedef uint64_t u64;

    struct EntityCoordinates {
        double x{0.0}, y{0.0}, z{0.0};
        u64 dimension{0};
    };

    struct BlockCoordinates {
        i64 x{0}, y{0}, z{0};
        u64 dimension{0};

        bool operator==(const BlockCoordinates& other) const noexcept {
            return x == other.x && y == other.y && z == other.z;
        }
    };

    struct ChunkCoordinates {
        i64 x{0}, y{0}, z{0};
        u64 dimension{0};

        bool operator==(const ChunkCoordinates& other) const noexcept {
            return x == other.x && y == other.y && z == other.z;
        }
    };
}