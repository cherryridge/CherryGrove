#pragma once
#include <cstdint>

namespace Gui {
    typedef uint8_t u8;

    enum struct Intrinsics : u8 {
        MainMenu,
        Copyright,
        Version,
        DebugMenu,
        Saves,
        ContentPacks,
        Settings,
        CreateWorld,
        ContentPackDetails,
        About,
        Inventory,
        
        Count
    };
}