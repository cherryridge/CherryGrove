#pragma once
#include <cstdint>

namespace Util::Json {
    typedef uint8_t u8;

    //note: Remember to update `schema.hpp` when registering a new kind, so we can generate the schema.
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