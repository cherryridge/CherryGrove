#pragma once
#include <atomic>
#include <format>
#include <imgui.h>

#include "../components/Coordinates.hpp"
#include "../components/Rotation.hpp"
#include "../../graphics/gui/util.hpp"
#include "../../simulation/playerEntity.hpp"
#include "../../simulation/Simulation.hpp"

namespace Gui::DebugMenu {
    using std::memory_order_acquire, std::format;

    inline void render() noexcept {
        Gui::Util::tlWindow("DebugMenu", true);
        if (Simulation::gameStarted.load(memory_order_acquire)) {
            const auto& coords = Simulation::playerEntity.get<Components::EntityCoordinates>();
            ImGui::TextUnformatted(format("({}, {}, {})", coords.x, coords.y, coords.z).c_str());
            const auto& rotation = Simulation::playerEntity.get<Components::Rotation>();
            ImGui::TextUnformatted(format("Yaw: {}, Pitch: {}", rotation.yaw, rotation.pitch).c_str());
        }
        else ImGui::TextUnformatted("Game stopped.");
        Gui::Util::endWindow(true);
    }
}