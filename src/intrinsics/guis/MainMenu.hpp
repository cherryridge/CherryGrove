#pragma once
#include <atomic>
#include <imgui.h>

#include "../../simulation/Simulation.hpp"
#include "../../sound/Sound.hpp"
#include "../../globalState.hpp"
#include "../../graphics/gui/Gui.hpp"
#include "../../graphics/gui/GuiUtils.hpp"

namespace Gui::MainMenu {
    using std::memory_order_release;
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
            centerButton(u8"存档（调试）", btnSize, []() noexcept {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
                Simulation::start();
            });
            centerButton(u8"内容包", btnSize, []() noexcept {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
            });
            centerButton(u8"设置", btnSize, []() noexcept {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
            });
            centerButton(u8"退出", btnSize, []() noexcept {
                static_cast<void>(Sound::play(Gui::click, Sound::DUMMY_COORD_2D));
                GlobalState::isCGAlive.store(false, memory_order_release);
            });
            PopStyleVar();
            PopFont();
        }
        endWindow();
    }
}