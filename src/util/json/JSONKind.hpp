#pragma once
#include <cstdint>

namespace Util::Json {
    typedef uint8_t u8;

    enum struct JSONKind : u8 {
        Settings,
        Manifest,
        //BlockDefinition,
        //TextureDefinition,
        //BlockGeometry,
        //EntityDefinition,
        //EntityGeometry,
        Count
    };

    [[nodiscard]] inline const char* getJSONKindName(JSONKind kind) noexcept {
        switch (kind) {
            using enum JSONKind;
            case Settings: return "settings";
            case Manifest: return "manifest";
            //case BlockDefinition: return "blockDefinition";
            //case TextureDefinition: return "textureDefinition";
            //case BlockGeometry: return "blockGeometry";
            //case EntityDefinition: return "entityDefinition";
            //case EntityGeometry: return "entityGeometry";
            default: return "Unknown";
        }
    }
}