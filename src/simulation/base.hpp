#pragma once
#include <glm/glm.hpp>

namespace Simulation {
    typedef int64_t i64;
    typedef uint64_t u64;
    using glm::vec, std::abs;

    struct ManhattanRange : private vec<3, i64> {
        using vec<3, i64>::x, vec<3, i64>::y, vec<3, i64>::z;
        u64 size;

        [[nodiscard]] bool contains(const vec<3, i64>& point) const noexcept { return static_cast<u64>(abs(point.x - x)) + static_cast<u64>(abs(point.y - y)) + static_cast<u64>(abs(point.z - z)) <= size; }
        [[nodiscard]] bool isInnerEdge(const vec<3, i64>& point) const noexcept { return static_cast<u64>(abs(point.x - x)) + static_cast<u64>(abs(point.y - y)) + static_cast<u64>(abs(point.z - z)) == size; }
        [[nodiscard]] bool isOuterEdge(const vec<3, i64>& point) const noexcept { return static_cast<u64>(abs(point.x - x)) + static_cast<u64>(abs(point.y - y)) + static_cast<u64>(abs(point.z - z)) == size + 1; }
        [[nodiscard]] bool intersects(const ManhattanRange& other) const noexcept { return static_cast<u64>(abs(other.x - x)) + static_cast<u64>(abs(other.y - y)) + static_cast<u64>(abs(other.z - z)) <= size + other.size; }
    };
}