#pragma once
#include <imgui.h>

#include "../../graphics/gui/util.hpp"
#include "../../meta.hpp"

namespace Gui::Copyright {
    inline void render() noexcept {
        Gui::Util::blWindow("Copyright", true, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav);
        ImGui::TextUnformatted(CG_COPYRIGHT_NOTICE);
        Gui::Util::endWindow(true);
    }
}