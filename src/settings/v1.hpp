#pragma once
#include <array>
#include <vector>

#include "base.hpp"
#include "PackOptionValue.hpp"

namespace Settings {
    typedef int16_t i16;
    typedef int64_t i64;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::array, std::vector;

    struct Settings_v1 {

    u32 formatVersion{1};

    struct Packs {
        vector<KnownPack> knownPacks;
        vector<PackOptionValue> packOptions;
        vector<string> additionalPackRoots, additionalPacks;
    } packs;

    struct Graphics {
        float fov{70.0f};
        float gamma{1.0f};
        //`0` is for unbounded.
        u32 maxFPS{60};
        u32 renderDistance{32};
        u32 windowWidth{1920};
        u32 windowHeight{1080};
        WindowMode windowMode{WindowMode::Windowed};
        bool vsync{true};
    } graphics;

    struct Simulation {
        u32 simulationDistance{16};
        u32 maxPlanckTimePerSec{20};
    } simulation;

    struct Input {
        float mouseSensitivity{1.0f};
        float scrollSensitivity{1.0f};
        u32 repeatTapGap{100};
        u32 comboMinTTL{100};
        array<i16, 8> deadzones{7800, 32767, 7800, 32767, 7800, 32767, 7800, 32767};
        bool invertMouseX{false};
        bool invertMouseY{false};
        bool swapSticks{false};
        vector<Keybind> keybinds;
        vector<OtherBind> gamepadBinds;
        vector<OtherBind> mouseMoveBinds;
        vector<OtherBind> scrollBinds;
        vector<OtherBind> stickBinds;
    } input;

    struct Debug {
        Logger::LoggingMode logging{Logger::LoggingMode::File};
    } debug;

    struct Misc {
        string windowTitleBase{"CherryGrove"};
        bool checkEngineUpdates{true};
    } misc;

    };
}