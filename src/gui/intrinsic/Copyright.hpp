#pragma once
#include <imgui.h>

#include "../GuiUtils.hpp"
#include "../../meta.hpp"

namespace Gui::Copyright {
    using namespace ImGui;
    using namespace GuiUtils;
    inline void render() {
        blWindow("Copyright");
        TextUnformatted(CG_COPYRIGHT_NOTICE);
        endWindow();
    }
}