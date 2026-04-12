#pragma once
#include <cstdint>
#include <backends/imgui_impl_sdl3.h>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/unordered/unordered_flat_set.hpp>
#include <function2/function2.hpp>
#include <imgui.h>
#include <SDL3/SDL.h>

#include "../../debug/Fatal.hpp"
#include "../../debug/Logger.hpp"
#include "../../intrinsics/guis/Copyright.hpp"
#include "../../intrinsics/guis/DebugMenu.hpp"
#include "../../intrinsics/guis/MainMenu.hpp"
#include "../../intrinsics/guis/Version.hpp"
#include "../../sound/Sound.hpp"
#include "../../window.hpp"
#include "../backend/imgui_impl_bgfx.hpp"
#include "../renderer/definitions.hpp"
#include "click.hpp"
#include "Intrinsics.hpp"

namespace Gui {
    typedef uint8_t u8;
    typedef int32_t i32;
    using boost::unordered_flat_map, boost::unordered_flat_set, fu2::function_view, Sound::SoundHandle;

    inline unordered_flat_set<Intrinsics> visibleGuis;
    inline unordered_flat_map<Intrinsics, function_view<void()>> guiRegistry;

    inline void init() noexcept {
        i32 width, height;
        SDL_GetWindowSize(Window::getMainWindow(), &width, &height);
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.LogFilename = nullptr;
        io.IniFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.DisplaySize.x = static_cast<float>(width);
        io.DisplaySize.y = static_cast<float>(height);
        io.ConfigViewportsNoAutoMerge = true;
        io.ConfigViewportsNoTaskBarIcon = true;
        //A legacy flag added before ImGui 1.92 which prevents ImGui from trying to create a very large texture for a large font atlas and crashing the app.
        //fixme: Need more information to determine if we can remove this line.
        io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
        io.Fonts->Clear();
        io.Fonts->AddFontFromFileTTF("assets/fonts/unifont.otf", 18.0f);
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.FontSizeBase = 18.0f;
        style.FontScaleDpi = SDL_GetWindowDisplayScale(Window::getMainWindow());
        style.WindowRounding = 0.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.0f);
        ImGui_ImplSDL3_InitForOther(Window::getMainWindow());
        if (!ImGui_Implbgfx_Init({
            .startingViewId = Renderer::VIEWID_STARTING_GUI,
            .maxViews = Renderer::VIEWIDS_GUI,
            .enableMultiViewport = true,
            .autoShaderSampler = true,
            .platformBridge = {
                .getNativeWindowHandle = Window::getNativeWindowHandle
            }
        })) {
            lerr << "Failed to initialize ImGui for bgfx!" << endl;
            Fatal::exit(Fatal::IMGUI_INITIALIZATION_FAILED);
        }
        click = Sound::addSound("assets/sounds/click1.ogg", false, true, 2.0f, Sound::FLOAT_INFINITY, Sound::FLOAT_INFINITY);
        //Register them manually!
        guiRegistry.emplace(Intrinsics::MainMenu, MainMenu::render);
        guiRegistry.emplace(Intrinsics::Copyright, Copyright::render);
        guiRegistry.emplace(Intrinsics::Version, Version::render);
        guiRegistry.emplace(Intrinsics::DebugMenu, DebugMenu::render);
    }

    inline void shutdown() noexcept {
        ImGui_Implbgfx_Shutdown();
        ImGui_ImplSDL3_Shutdown();
    }

    inline void render() noexcept {
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        unordered_flat_set<Intrinsics> visibleGuisBuffer = visibleGuis;
        for (const auto& gui : visibleGuisBuffer) {
            if (guiRegistry.find(gui) != guiRegistry.end()) guiRegistry[gui]();
            else lerr << "[GUI] Did you forget to emplace the GUI: " << static_cast<i32>(gui) << "?" << endl;
        }
        ImGui::Render();
        ImGui_Implbgfx_RenderDrawData(ImGui::GetDrawData());
    }

    inline void setVisibility(Intrinsics gui, bool visible) noexcept {
        if (visible && visibleGuis.find(gui) == visibleGuis.end()) visibleGuis.insert(gui);
        else if (!visible && visibleGuis.find(gui) != visibleGuis.end()) visibleGuis.erase(gui);
    }
}