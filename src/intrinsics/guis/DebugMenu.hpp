#pragma once
#include <atomic>
#include <format>
#include <string>
#include <imgui.h>

#include "../../components/Components.hpp"
#include "../../graphics/gui/GuiUtils.hpp"
#include "../../simulation/Simulation.hpp"

namespace Gui::DebugMenu {
    using namespace ImGui;
    using namespace GuiUtils;
    using std::memory_order_acquire, std::format, std::string, std::to_string;

    inline void render() noexcept {
        tlWindow("DebugMenu");
        if (Simulation::gameStarted.load(memory_order_acquire)) {
            auto& coords = Simulation::gameRegistry.get<Components::CoordinatesComp>(Simulation::playerEntity);
            TextUnformatted(format("({}, {}, {})", coords.x, coords.y, coords.z).c_str());
            auto& rotation = Simulation::gameRegistry.get<Components::RotationComp>(Simulation::playerEntity);
            TextUnformatted(format("Yaw: {}, Pitch: {}", rotation.yaw, rotation.pitch).c_str());
        }
        else TextUnformatted("Game stopped.");
        endWindow();
    }
}