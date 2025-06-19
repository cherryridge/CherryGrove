#pragma once
#include <imgui.h>

#include "../CherryGrove.hpp"
#include "../../sound/Sound.hpp"
#include "../../simulation/Simulation.hpp"
#include "../GuiUtils.hpp"
#include "../Gui.hpp"

namespace Gui::MainMenu {
    using namespace ImGui;
    using namespace GuiUtils;
    using Gui::click;

    inline void render() {
        centerWindow("MainMenu");
        {
            PushFont(GetIO().Fonts->Fonts[1]);
            PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 80.0f));
            centerText("CherryGrove");
            PopStyleVar();
            PopFont();
        }
        {
            PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 30.0f));
            ImVec2 btnSize(240.0f, 80.0f);
            centerButton(u8"存档（调试）", btnSize, []() {
                Sound::play(click, Sound::DUMMY_COORD_2D);
                Simulation::start();
            });
            centerButton(u8"内容包", btnSize, []() {
                Sound::play(click, Sound::DUMMY_COORD_2D);
            });
            centerButton(u8"设置", btnSize, []() {
                Sound::play(click, Sound::DUMMY_COORD_2D);
            });
            centerButton(u8"退出", btnSize, []() {
                Sound::play(click, Sound::DUMMY_COORD_2D);
                CherryGrove::isCGAlive = false;
            });
            PopStyleVar();
        }
        endWindow();
    }
}