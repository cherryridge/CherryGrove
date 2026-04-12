#pragma once
#include <imgui.h>

#include "../../graphics/gui/util.hpp"
#include "../../meta.hpp"

namespace Gui::Copyright {
    inline void render() noexcept {
        Gui::Util::blWindow("Copyright", true);
        ImGui::TextUnformatted(CG_COPYRIGHT_NOTICE);
        Gui::Util::endWindow(true);
    }
}