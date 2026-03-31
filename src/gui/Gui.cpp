//Temporary
#pragma execution_character_set(push, "utf-8")

#include <functional>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <imgui.h>
#include <backends/imgui_impl_bgfx.h>
#include <backends/imgui_impl_sdl3.h>
#include <SDL3/SDL.h>

#include "../Main.hpp"
#include "../debug/Logger.hpp"
#include "../graphics/Renderer.hpp"
#include "../settings/Settings.hpp"
#include "../sound/Sound.hpp"
#include "intrinsic/Copyright.hpp"
#include "intrinsic/DebugMenu.hpp"
#include "intrinsic/MainMenu.hpp"
#include "intrinsic/Version.hpp"
#include "Gui.hpp"

namespace Gui {
    typedef int32_t i32;
    typedef uint32_t u32;
    using std::function, boost::unordered_flat_set, boost::unordered_flat_map, Sound::SoundHandle;

    static ImGuiContext* context;
    static unordered_flat_set<Intrinsics> visibleGuis;
    static unordered_flat_map<Intrinsics, function<void()>> guiRegistry;
    SoundHandle click{};

    void init() noexcept {
        i32 width, height;
        SDL_GetWindowSize(Main::windowHandle, &width, &height);
        IMGUI_CHECKVERSION();
        context = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.LogFilename = nullptr;
        io.IniFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.DisplaySize.x = static_cast<float>(width);
        io.DisplaySize.y = static_cast<float>(height);
        io.ConfigViewportsNoAutoMerge = true;
        io.ConfigViewportsNoTaskBarIcon = true;
        float scale = SDL_GetWindowDisplayScale(Main::windowHandle);
        //todo: Rework fonts!!!
        io.FontGlobalScale = scale;
        float scaledFontSize = 24.0f * scale;
        io.Fonts->Flags |= ImFontAtlasFlags_::ImFontAtlasFlags_NoPowerOfTwoHeight;
        io.Fonts->Clear();
        io.FontGlobalScale = 1.0f;
        // todo: manage texture manually
        io.Fonts->AddFontFromFileTTF("assets/fonts/unifont.otf", scaledFontSize, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
        io.Fonts->AddFontFromFileTTF("assets/fonts/unifont.otf", 1.4f * scaledFontSize, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
        //io.Fonts->Build();
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.0f);
        //Gemini recommends not to use `msaaSamples` whatsoever, so we're just vibing it with `1`.
        //todo: Populate `bUsingVSync` from Settings.
        ImGui_Implbgfx_Init(Renderer::guiViewId, 1, Settings::getData().graphics.vsync);
        ImGui_ImplSDL3_InitForOther(Main::windowHandle);
        click = Sound::addSound("assets/sounds/click1.ogg", false, true, 2.0f, Sound::FLOAT_INFINITY, Sound::FLOAT_INFINITY);
        //Register them manually!
        guiRegistry.emplace(Intrinsics::MainMenu, MainMenu::render);
        guiRegistry.emplace(Intrinsics::Copyright, Copyright::render);
        guiRegistry.emplace(Intrinsics::Version, Version::render);
        guiRegistry.emplace(Intrinsics::DebugMenu, DebugMenu::render);
    }

    void shutdown() noexcept {
        ImGui_Implbgfx_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext(context);
    }

    void render() noexcept {
        ImGui_Implbgfx_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        unordered_flat_set<Intrinsics> visibleGuisBuffer = visibleGuis;
        for (const auto& gui : visibleGuisBuffer) {
            if (guiRegistry.find(gui) != guiRegistry.end()) guiRegistry[gui]();
            else lerr << "[GUI] Did you forget to emplace the GUI: " << static_cast<i32>(gui) << "?" << endl;
        }
        ImGui::Render();
        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
    }

    void setVisible(Intrinsics gui, bool visible) {
        if (visible && visibleGuis.find(gui) == visibleGuis.end()) visibleGuis.insert(gui);
        else if (!visible && visibleGuis.find(gui) != visibleGuis.end()) visibleGuis.erase(gui);
    }
}