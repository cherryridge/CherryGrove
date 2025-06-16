﻿#pragma once

#include "../../simulation/Simulation.hpp"

namespace Gui::DebugMenu {
    using namespace ImGui;
    using namespace GuiUtils;
    void render() {
        tlWindow("DebugMenu");
        if (Simulation::gameStarted) {
            auto& coords = Simulation::gameRegistry.get<Components::CoordinatesComp>(Simulation::playerEntity);
            std::string coordsStr = "(";
            coordsStr += std::to_string(coords.x);
            coordsStr += ", ";
            coordsStr += std::to_string(coords.y);
            coordsStr += ", ";
            coordsStr += std::to_string(coords.z);
            coordsStr += ")";
            TextUnformatted(coordsStr.c_str());
            auto& rotation = Simulation::gameRegistry.get<Components::RotationComp>(Simulation::playerEntity);
            std::string rotationStr = "Yaw: ";
            rotationStr += std::to_string(rotation.yaw);
            rotationStr += " Pitch: ";
            rotationStr += std::to_string(rotation.pitch);
            TextUnformatted(rotationStr.c_str());
        }
        else TextUnformatted("Game stopped.");
        endWindow();
    }
}