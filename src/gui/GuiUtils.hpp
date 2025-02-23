#pragma once
#include <imgui.h>
#include <functional>

namespace Guis::Utils {
	constexpr ImVec2 zero2(0.0f, 0.0f);
	constexpr ImVec4 zero4(0.0f, 0.0f, 0.0f, 0.0f);
	constexpr const ImVec4& transparent = zero4;
	constexpr ImVec4 debugTP(0.0f, 0.0f, 0.0f, 0.1f);

	void blWindow(const char* id);
	void brWindow(const char* id);
	void centerWindow(const char* id);
	void endWindow();

	void centerText(const char* text);
	//Don't forget `u8""` while using CJK characters.
	void centerButton(const char8_t* text, ImVec2 btnSize, std::function<void()> clickCB);
}