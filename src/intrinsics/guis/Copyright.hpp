#pragma once
#include <imgui.h>

#include "../../graphics/gui/GuiUtils.hpp"
#include "../../meta.hpp"

namespace Gui::Copyright {
    using namespace ImGui;
    using namespace GuiUtils;

    inline void render() noexcept {
        blWindow("Copyright");
        TextUnformatted(CG_COPYRIGHT_NOTICE);
        endWindow();
    }
}