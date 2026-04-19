#pragma once
#include <string_view>
#include <imgui.h>

#include "../../simulation/Simulation.hpp"
#include "../../sound/Sound.hpp"
#include "../../globalState.hpp"
#include "../../graphics/gui/click.hpp"
#include "../../graphics/gui/util.hpp"

namespace Gui::MainMenu {
    using std::string_view;

    inline void render() noexcept {
        Gui::Util::centerWindow("MainMenu", true);
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0], 28.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 80.0f));
            Gui::Util::centerText("CherryGrove");
            ImGui::PopStyleVar();
            ImGui::PopFont();
        }
        {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0], 18.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 30.0f));
            ImVec2 btnSize(240.0f, 80.0f);
            Gui::Util::centerButton("存档", btnSize, []() noexcept {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
                Simulation::start();
            });
            Gui::Util::centerButton("内容包", btnSize, []() noexcept {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
            });
            Gui::Util::centerButton("设置", btnSize, []() noexcept {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
            });
            Gui::Util::centerButton("退出", btnSize, []() noexcept {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
                GlobalState::setIsCGAlive(false);
            });
            ImGui::PopStyleVar();
            ImGui::PopFont();
        }
        Gui::Util::endWindow(true);
    }
}