#pragma once
#include <cstdint>

#include "../../sound/types.hpp"

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

    extern Sound::SoundHandle click;

    //Must be called after `Sound::init`.
    void init() noexcept;
    void shutdown() noexcept;

    void setVisibility(Intrinsics gui, bool visible) noexcept;
    void render() noexcept;
}