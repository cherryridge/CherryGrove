#pragma once
#include <functional>
#include <imgui.h>

namespace GuiUtils {
    using std::function;
    #define CJK(str) reinterpret_cast<const char*>(str)

    inline constexpr ImVec2 zero2(0.0f, 0.0f);
    inline constexpr ImVec4 zero4(0.0f, 0.0f, 0.0f, 0.0f);
    inline constexpr const ImVec4& transparent = zero4;
    inline constexpr ImVec4 debugTP(0.0f, 0.0f, 0.0f, 0.1f);

    void tlWindow(const char* id, bool styled = true) noexcept;
    void blWindow(const char* id, bool styled = true) noexcept;
    void brWindow(const char* id, bool styled = true) noexcept;
    void centerWindow(const char* id, bool styled = true) noexcept;
    void endWindow(bool styled = true) noexcept;

    void centerText(const char8_t* text) noexcept;
    void centerText(const char* text) noexcept;
    //Don't forget `u8""` while using CJK characters.
    void centerButton(const char8_t* text, ImVec2 btnSize, function<void()> clickCB) noexcept;
}