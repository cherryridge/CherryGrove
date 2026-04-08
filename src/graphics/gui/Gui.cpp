#include <backends/imgui_impl_bgfx.h>
#include <backends/imgui_impl_sdl3.h>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <function2/function2.hpp>
#include <imgui.h>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"
#include "../../intrinsics/guis/Copyright.hpp"
#include "../../intrinsics/guis/DebugMenu.hpp"
#include "../../intrinsics/guis/MainMenu.hpp"
#include "../../intrinsics/guis/Version.hpp"
#include "../../settings/Settings.hpp"
#include "../../globalState.hpp"
#include "../../sound/Sound.hpp"
#include "../renderer/Renderer.hpp"
#include "Gui.hpp"

namespace Gui {
    typedef int32_t i32;
    typedef uint32_t u32;
    using fu2::function_view, boost::unordered_flat_set, boost::unordered_flat_map, Sound::SoundHandle;

    static ImGuiContext* context;
    static unordered_flat_set<Intrinsics> visibleGuis;
    static unordered_flat_map<Intrinsics, function_view<void()>> guiRegistry;
    SoundHandle click{};

    void init() noexcept {
        i32 width, height;
        SDL_GetWindowSize(GlobalState::windowHandle, &width, &height);
        IMGUI_CHECKVERSION();
        context = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.LogFilename = nullptr;
        io.IniFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.DisplaySize.x = static_cast<float>(width);
        io.DisplaySize.y = static_cast<float>(height);
        io.ConfigViewportsNoAutoMerge = true;
        io.ConfigViewportsNoTaskBarIcon = true;
        io.Fonts->Flags |= ImFontAtlasFlags_::ImFontAtlasFlags_NoPowerOfTwoHeight;
        io.Fonts->Clear();
        io.Fonts->AddFontFromFileTTF("assets/fonts/unifont.otf", 18.0f);
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.FontSizeBase = 18.0f;
        style.FontScaleDpi = SDL_GetWindowDisplayScale(GlobalState::windowHandle);
        style.WindowRounding = 0.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.0f);
        //Gemini recommends not to use `msaaSamples` whatsoever, so we're just vibing it with `1`.
        ImGui_Implbgfx_Init(Renderer::guiViewId, 1, Settings::getSettings().graphics.vsync);
        ImGui_ImplSDL3_InitForOther(GlobalState::windowHandle);
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

    void setVisibility(Intrinsics gui, bool visible) noexcept {
        if (visible && visibleGuis.find(gui) == visibleGuis.end()) visibleGuis.insert(gui);
        else if (!visible && visibleGuis.find(gui) != visibleGuis.end()) visibleGuis.erase(gui);
    }
}