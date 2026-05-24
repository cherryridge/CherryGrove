#pragma once
#include <atomic>
#include <format>
#include <imgui.h>

#include "../../graphics/gui/util.hpp"
#include "../../simulation/playerEntity.hpp"
#include "../../simulation/states.hpp"
#include "../components/Coordinates.hpp"
#include "../components/Rotation.hpp"

namespace Gui::DebugMenu {
    using std::memory_order_acquire, std::format;

    inline void render() noexcept {
        Gui::Util::tlWindow("DebugMenu", true, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav);
        if (Simulation::isSimStarted()) {
            const auto& coords = Simulation::playerEntity.get<Components::EntityCoordinates>();
            ImGui::TextUnformatted(format("\n(X: {:.2f}, Y: {:.2f}, Z: {:.2f})", coords.x, coords.y, coords.z).c_str());
            const auto& rotation = Simulation::playerEntity.get<Components::Rotation>();
            ImGui::TextUnformatted(format("(Y: {:.2f}, P: {:.2f})", rotation.yaw, rotation.pitch).c_str());
        }
        else ImGui::TextUnformatted("Game stopped.");
        Gui::Util::endWindow(true);
    }
}