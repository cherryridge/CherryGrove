#pragma once
#include <cstdint>

#include "../sound/Sound.hpp"

namespace Gui {
    typedef uint8_t u8;
    typedef int32_t i32;

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

    extern Sound::EventID click;

    //Must be called after `Sound::init`.
    void init() noexcept;
    void shutdown() noexcept;

    void setVisible(Intrinsics gui, bool visible = true);
    void render() noexcept;
}