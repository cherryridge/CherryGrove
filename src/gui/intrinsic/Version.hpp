#pragma once
#include <imgui.h>

#include "../GuiUtils.hpp"

namespace Gui::Version {
    using namespace ImGui;
    using namespace GuiUtils;
    inline void render() {
        brWindow("Version");
        TextUnformatted("0.0.1");
        endWindow();
    }
}