#pragma once
#include <cstdint>

namespace Util::NBT {
    typedef uint8_t u8;

    enum struct NBTKind : u8 {
        WorldInfo,
        Count
    };

    [[nodiscard]] inline const char* getNBTKindName(NBTKind kind) noexcept {
        switch (kind) {
            using enum NBTKind;
            case WorldInfo: return "worldInfo";
            default: return "Unknown";
        }
    }
}