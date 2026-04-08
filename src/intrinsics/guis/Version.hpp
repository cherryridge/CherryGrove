#pragma once
#include <imgui.h>

#include "../../graphics/gui/GuiUtils.hpp"
#include "../../meta.hpp"

namespace Gui::Version {
    using namespace ImGui;
    using namespace GuiUtils;

    inline void render() noexcept {
        brWindow("Version");
        TextUnformatted(CG_SEMATIC_VERSION);
        endWindow();
    }
}