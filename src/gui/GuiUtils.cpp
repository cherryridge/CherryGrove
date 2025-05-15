#include <functional>
#include <imgui.h>

#include "Guis.hpp"
#include "GuiUtils.hpp"

namespace Guis::Utils {
    using Guis::cache, std::function;
    using namespace ImGui;

    static void windowBase(const char* id, bool styled = true) noexcept {
        if(styled) {
            PushStyleColor(ImGuiCol_WindowBg, transparent);
            PushStyleColor(ImGuiCol_Border, transparent);
        }
        Begin(id, NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
        auto drawList = GetWindowDrawList();
        drawList->PushClipRectFullScreen();
    }

    void tlWindow(const char* id, bool styled) noexcept {
        windowBase(id, styled);
        auto size = GetWindowSize();
        SetWindowPos(ImVec2(0.0f, 0.0f));
    }

    void blWindow(const char* id, bool styled) noexcept {
        windowBase(id, styled);
        auto size = GetWindowSize();
        SetWindowPos(ImVec2(0.0f, cache.height - size.y));
    }

    void brWindow(const char* id, bool styled) noexcept {
        windowBase(id, styled);
        auto size = GetWindowSize();
        SetWindowPos(ImVec2(cache.width - size.x, cache.height - size.y));
    }

    void centerWindow(const char* id, bool styled) noexcept {
        windowBase(id, styled);
        auto size = GetWindowSize();
        SetWindowPos(ImVec2((cache.width - size.x) / 2.0f, (cache.height - size.y) / 2.0f));
    }

    void endWindow(bool styled) noexcept {
        auto drawList = GetWindowDrawList();
        drawList->PopClipRect();
        if(styled) PopStyleColor(2);
        End();
    }

    void centerText(const char8_t* text) noexcept {
        auto textSize = CalcTextSize(reinterpret_cast<const char*>(text)), winSize = GetWindowSize();
        SetCursorPosX((winSize.x - textSize.x) / 2.0f);
        TextUnformatted(reinterpret_cast<const char*>(text));
    }

    void centerText(const char* text) noexcept {
        auto textSize = CalcTextSize(text), winSize = GetWindowSize();
        SetCursorPosX((winSize.x - textSize.x) / 2.0f);
        TextUnformatted(text);
    }

    void centerButton(const char8_t* text, ImVec2 btnSize, function<void()> clickCB) noexcept {
        auto winSizeX = GetWindowSize().x;
        SetCursorPosX((winSizeX - btnSize.x) / 2.0f);
        if (Button(reinterpret_cast<const char*>(text), btnSize)) clickCB();
    }
}