#include <imgui.h>
#include <functional>

#include "Guis.hpp"
#include "GuiUtils.hpp"

namespace Guis::Utils {
	using Guis::cache, std::function;
	using namespace ImGui;

	static void windowBase(const char* id) {
		PushStyleColor(ImGuiCol_WindowBg, transparent);
		PushStyleColor(ImGuiCol_Border, transparent);
		Begin(id, NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
		auto drawList = ImGui::GetWindowDrawList();
		drawList->PushClipRectFullScreen();
	}

	void blWindow(const char* id) {
		windowBase(id);
		auto size = GetWindowSize();
		SetWindowPos(ImVec2(0.0f, cache.height - size.y));
	}

	void brWindow(const char* id) {
		windowBase(id);
		auto size = GetWindowSize();
		SetWindowPos(ImVec2(cache.width - size.x, cache.height - size.y));
	}

	void centerWindow(const char* id) {
		windowBase(id);
		auto size = GetWindowSize();
		SetWindowPos(ImVec2((cache.width - size.x) / 2.0f, (cache.height - size.y) / 2.0f));
	}

	void endWindow() {
		auto drawList = ImGui::GetWindowDrawList();
		drawList->PopClipRect();
		PopStyleColor(2);
		End();
	}

	void centerText(const char* text) {
		auto textSize = CalcTextSize(text), winSize = GetWindowSize();
		SetCursorPosX((winSize.x - textSize.x) / 2.0f);
		Text(text);
	}

	void centerButton(const char8_t* text, ImVec2 btnSize, function<void()> clickCB) {
		auto winSizeX = GetWindowSize().x;
		SetCursorPosX((winSizeX - btnSize.x) / 2.0f);
		if (Button(reinterpret_cast<const char*>(text), btnSize)) clickCB();
	}
}