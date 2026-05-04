#pragma once
#include <cstdint>
#include <cstdlib>
#include <glm/glm.hpp>

namespace Simulation {
    typedef int64_t i64;
    typedef uint64_t u64;
    using std::abs, glm::vec3;

    struct ManhattanRange {
        i64 x, y, z;
        u64 size;

        [[nodiscard]] bool contains(const vec<3, i64>& point) const noexcept { return static_cast<u64>(abs(point.x - x)) + static_cast<u64>(abs(point.y - y)) + static_cast<u64>(abs(point.z - z)) <= size; }
        //Is the point right on the inner side of the edge of the range? (i.e. is it in the range, but if we shrink the range by 1, it won't be in the range anymore?)
        [[nodiscard]] bool isEdgeInner(const vec<3, i64>& point) const noexcept { return static_cast<u64>(abs(point.x - x)) + static_cast<u64>(abs(point.y - y)) + static_cast<u64>(abs(point.z - z)) == size; }
        //Is the point right on the outer side of the edge of the range? (i.e. is it outside the range, but if we expand the range by 1, it will be in the range?)
        [[nodiscard]] bool isEdgeOuter(const vec<3, i64>& point) const noexcept { return static_cast<u64>(abs(point.x - x)) + static_cast<u64>(abs(point.y - y)) + static_cast<u64>(abs(point.z - z)) == size + 1; }
        [[nodiscard]] bool intersects(const ManhattanRange& other) const noexcept { return static_cast<u64>(abs(other.x - x)) + static_cast<u64>(abs(other.y - y)) + static_cast<u64>(abs(other.z - z)) <= size + other.size; }
    };
}