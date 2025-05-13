#pragma execution_character_set(push, "utf-8")
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <imgui.h>

#include "../debug/Logger.hpp"
#include "../sound/Sound.hpp"
#include "../CherryGrove.hpp"
#include "../MainGame.hpp"
#include "GuiUtils.hpp"
#include "Guis.hpp"

namespace Guis {
    typedef int32_t i32;
    typedef uint32_t u32;

    using Sound::EventID, std::unordered_map, std::unordered_set, std::function;

    EventID click;
    unordered_set<GuiWindow> visibleGuis;
    unordered_map<GuiWindow, function<void()>> guiRegistry;
    WindowInfoCache cache;

    namespace MainMenu {
        using namespace ImGui;
        using namespace Utils;
        void render() {
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
                    Sound::play(click);
                    MainGame::start();
                });
                centerButton(u8"内容包", btnSize, []() {
                    Sound::play(click);
                });
                centerButton(u8"设置", btnSize, []() {
                    Sound::play(click);
                });
                centerButton(u8"退出", btnSize, []() {
                    Sound::play(click);
                    CherryGrove::isCGAlive = false;
                });
                PopStyleVar();
            }
            endWindow();
        }
    }

    namespace Copyright {
        using namespace ImGui;
        using namespace Utils;
        void render() {
            blWindow("Copyright");
            TextUnformatted(reinterpret_cast<const char*>(u8"© 2025 LJM12914. Licensed under GPL-3.0-or-later."));
            endWindow();
        }
    }

    namespace Version {
        using namespace ImGui;
        using namespace Utils;
        void render() {
            brWindow("Version");
            TextUnformatted(reinterpret_cast<const char*>(u8"0.0.1"));
            endWindow();
        }
    }

    namespace DebugMenu {
        using namespace ImGui;
        using namespace Utils;
        void render() {
            tlWindow("DebugMenu");
            auto& coords = MainGame::gameRegistry.get<Components::CoordinatesComponent>(MainGame::playerEntity);
            std::string coordsStr = "(";
            coordsStr += std::to_string(coords.x);
            coordsStr += ", ";
            coordsStr += std::to_string(coords.y);
            coordsStr += ", ";
            coordsStr += std::to_string(coords.z);
            coordsStr += ")";
            TextUnformatted(coordsStr.c_str());
            auto& rotation = MainGame::gameRegistry.get<Components::RotationComponent>(MainGame::playerEntity);
            std::string rotationStr = "Yaw: ";
            rotationStr += std::to_string(rotation.yaw);
            rotationStr += " Pitch: ";
            rotationStr += std::to_string(rotation.pitch);
            TextUnformatted(rotationStr.c_str());
            endWindow();
        }
    }

    void init() {
        click = Sound::addEvent(Sound::addSound("assets/sounds/click1.ogg"), 2.0f, 1.0f, 0.0f, true);
        //Register them manually!
        guiRegistry.emplace(GuiWindow::wMainMenu, &MainMenu::render);
        guiRegistry.emplace(GuiWindow::wCopyright, &Copyright::render);
        guiRegistry.emplace(GuiWindow::wVersion, &Version::render);
        guiRegistry.emplace(GuiWindow::wDebugMenu, &DebugMenu::render);
    }

    void render(i32 width, i32 height) {
        cache = {width, height, (float)width / height};
        unordered_set<GuiWindow> visibleGuisBuffer = visibleGuis;
        for (const auto& gui : visibleGuisBuffer) {
            if (guiRegistry.find(gui) != guiRegistry.end()) guiRegistry[gui]();
            else lerr << "[GUI] Did you forget to emplace the GUI? " << gui << endl;
        }
    }

    void setVisible(GuiWindow gui, bool visible) {
        if (visible && visibleGuis.find(gui) == visibleGuis.end()) visibleGuis.insert(gui);
        else if (!visible && visibleGuis.find(gui) != visibleGuis.end()) visibleGuis.erase(gui);
    }
}