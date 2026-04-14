#pragma once
#include <string_view>
#include <function2/function2.hpp>
#include <imgui.h>

namespace Gui::Util {
    using std::string_view, fu2::function_view;

    inline constexpr ImVec2 zero2{0.0f, 0.0f};
    inline constexpr ImVec4
        zero4{0.0f, 0.0f, 0.0f, 0.0f},
        transparent{0.0f, 0.0f, 0.0f, 0.0f},
        debugTransparent{0.0f, 0.0f, 0.0f, 0.2f};

    namespace detail {
        inline void windowBase(string_view id, bool styled) noexcept {
            if (styled) {
                ImGui::PushStyleColor(ImGuiCol_WindowBg, transparent);
                ImGui::PushStyleColor(ImGuiCol_Border, transparent);
            }
            ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
            ImGui::Begin(id.data(), NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
            auto drawList = ImGui::GetWindowDrawList();
            drawList->PushClipRectFullScreen();
        }
    }

    inline void tlWindow(string_view id, bool styled) noexcept {
        detail::windowBase(id, styled);
        const auto mainPos = ImGui::GetMainViewport()->Pos;
        ImGui::SetWindowPos(mainPos);
    }

    inline void blWindow(string_view id, bool styled) noexcept {
        detail::windowBase(id, styled);
        const auto winSize = ImGui::GetWindowSize(), mainPos = ImGui::GetMainViewport()->Pos, mainSize = ImGui::GetMainViewport()->Size;
        ImGui::SetWindowPos(ImVec2(mainPos.x, mainPos.y + mainSize.y - winSize.y));
    }

    inline void brWindow(string_view id, bool styled) noexcept {
        detail::windowBase(id, styled);
        const auto winSize = ImGui::GetWindowSize(), mainPos = ImGui::GetMainViewport()->Pos, mainSize = ImGui::GetMainViewport()->Size;
        ImGui::SetWindowPos(ImVec2(mainPos.x + mainSize.x - winSize.x, mainPos.y + mainSize.y - winSize.y));
    }

    inline void centerWindow(string_view id, bool styled) noexcept {
        detail::windowBase(id, styled);
        const auto winSize = ImGui::GetWindowSize(), mainPos = ImGui::GetMainViewport()->Pos, mainSize = ImGui::GetMainViewport()->Size;
        ImGui::SetWindowPos(ImVec2(mainPos.x + (mainSize.x - winSize.x) / 2.0f, mainPos.y + (mainSize.y - winSize.y) / 2.0f));
    }
    
    inline void endWindow(bool styled) noexcept {
        auto drawList = ImGui::GetWindowDrawList();
        drawList->PopClipRect();
        if (styled) ImGui::PopStyleColor(2);
        ImGui::End();
    }
    
    inline void centerText(string_view str) noexcept {
        auto textSize = ImGui::CalcTextSize(str.data()), winSize = ImGui::GetWindowSize();
        ImGui::SetCursorPosX((winSize.x - textSize.x) / 2.0f);
        ImGui::TextUnformatted(str.data());
    }
    
    inline void centerButton(string_view str, ImVec2 btnSize, function_view<void()> clickCB) noexcept {
        auto winSizeX = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((winSizeX - btnSize.x) / 2.0f);
        if (ImGui::Button(str.data(), btnSize)) clickCB();
    }
}