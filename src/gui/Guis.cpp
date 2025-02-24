#pragma execution_character_set("utf-8")
#include <cstdint>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>

#include "../debug/debug.hpp"
#include "../sound/Sound.hpp"
#include "../CherryGrove.hpp"
#include "../gameplay/MainGame.hpp"
#include "MainWindow.hpp"
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
			centerWindow("mainmenu");
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
			blWindow("copyright");
			Text(reinterpret_cast<const char*>(u8"© 2025 LJM12914. Licensed under GPL-3.0-or-later."));
			endWindow();
		}
	}

	namespace Version {
		using namespace ImGui;
		using namespace Utils;
		void render() {
			brWindow("version");
			Text(reinterpret_cast<const char*>(u8"0.0.1"));
			endWindow();
		}
	}

	void init() {
		click = Sound::addEvent(Sound::addSound("assets/sounds/click1.ogg"), 2.0f, 1.0f, 0.0f, true);
		//Register them manually!
		guiRegistry.emplace(GuiWindow::wMainMenu, &MainMenu::render);
		guiRegistry.emplace(GuiWindow::wCopyright, &Copyright::render);
		guiRegistry.emplace(GuiWindow::wVersion, &Version::render);
	}

	void render() {
		i32 width, height;
		glfwGetWindowSize(MainWindow::window, &width, &height);
		cache = {(u32)width, (u32)height, (float)width / height};
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