#pragma once
#include <imgui.h>

#include "../../graphics/gui/util.hpp"
#include "../../meta.hpp"

namespace Gui::Version {
    inline void render() noexcept {
        Gui::Util::brWindow("Version", true);
        ImGui::TextUnformatted(CG_SEMANTIC_VERSION);
        Gui::Util::endWindow(true);
    }
}