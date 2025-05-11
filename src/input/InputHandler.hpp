#pragma once
#include <map>

#include "inputBase.hpp"

#include "ImGuiAdapter.hpp"

#include "sources/boolInput.hpp"
#include "sources/mouseMove.hpp"
#include "sources/scroll.hpp"

namespace InputHandler {
    void init();
    void processInputGame();
    void processInputRenderer();
}