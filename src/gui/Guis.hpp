#pragma once
#include <cstdint>

namespace Guis {
    typedef int32_t i32;

    enum GuiWindow {
        wMainMenu,
        wCopyright,
        wVersion,
        wDebugMenu,
        wSaves,
        wContentPacks,
        wSettings,
        wCreateWorld,
        wContentPackDetails,
        wAbout,
        wInventory,
        
        wCount
    };

    struct WindowInfoCache {
        i32 width;
        i32 height;
        float aspectRatio;
    };

    //For GUI utils to access the cache.
    extern WindowInfoCache cache;

    //Must be called after `Sound::init`.
    void init();

    void setVisible(GuiWindow gui, bool visible = true);
    void render(i32 width, i32 height);
}