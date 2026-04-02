#pragma once
#include <imgui.h>

#include "../../simulation/Simulation.hpp"
#include "../../sound/Sound.hpp"
#include "../../gui/Gui.hpp"
#include "../../gui/GuiUtils.hpp"

namespace Gui::MainMenu {
    using namespace ImGui;
    using namespace GuiUtils;

    inline void render() noexcept {
        centerWindow("MainMenu");
        {
            PushFont(GetIO().Fonts->Fonts[0], 28.0f);
            PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 80.0f));
            centerText("CherryGrove");
            PopStyleVar();
            PopFont();
        }
        {
            PushFont(GetIO().Fonts->Fonts[0], 18.0f);
            PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 30.0f));
            ImVec2 btnSize(240.0f, 80.0f);
            centerButton(u8"存档（调试）", btnSize, []() {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
                Simulation::start();
            });
            centerButton(u8"内容包", btnSize, []() {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
            });
            centerButton(u8"设置", btnSize, []() {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
            });
            centerButton(u8"退出", btnSize, []() {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
                Main::isCGAlive = false;
            });
            PopStyleVar();
            PopFont();
        }
        endWindow();
    }
}