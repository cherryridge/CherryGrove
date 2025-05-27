#pragma once
#include <imgui.h>

#include "../GuiUtils.hpp"

namespace Gui::Copyright {
    using namespace ImGui;
    using namespace GuiUtils;
    inline void render() {
        blWindow("Copyright");
        TextUnformatted(CJK(u8"© 2025 LJM12914. Licensed under GPL-3.0-or-later."));
        endWindow();
    }
}