#pragma once

#include "../util/wrappers/uuid.hpp"

namespace Pack {
    typedef uint32_t u32;
    using Util::Wrapper::uuid_JSON;

    //[min, max], both inclusive. If `max` is `0`, it means there is no upper bound. Use `min == 1` for the same intent.
    struct PackVersionRange {
        uuid_JSON id;
        u32 min, max;

        [[nodiscard]] bool operator==(const PackVersionRange& other) const noexcept {
            return id == other.id && min == other.min && max == other.max;
        }

        [[nodiscard]] bool contains(const uuid_JSON& packId, u32 packVersion) const noexcept {
            if (id != packId) return false;
            if (max == 0) return packVersion >= min;
            else return packVersion >= min && packVersion <= max;
        }
    };
}