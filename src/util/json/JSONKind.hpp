#pragma once
#include <cstdint>

namespace Util::Json {
    typedef uint8_t u8;

    enum struct JSONKind : u8 {
        Settings,
        Manifest,
        BlockDefinition,
        TextureDefinition,
        BlockGeometry,
        EntityDefinition,
        EntityGeometry,
        Count
    };
}