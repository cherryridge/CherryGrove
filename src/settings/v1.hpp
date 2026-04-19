#pragma once
#include <array>
#include <string_view>
#include <vector>
#include <glaze/glaze.hpp>

#include "../pack/KnownPack.hpp"
#include "../pack/PackOptionValue.hpp"
#include "base.hpp"

namespace Settings {
    typedef int16_t i16;
    typedef int64_t i64;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::array, std::string_view, std::vector, glz::schema, Pack::KnownPack, Pack::PackOptionValue;

    JSON_STRUCT Settings_v1 {

    u32 formatVersion{1};

    JSON_STRUCT Packs {
        vector<KnownPack> knownPacks;
        vector<PackOptionValue> packOptions;
        vector<string> additionalPackRoots;
        vector<string> additionalPacks;

        struct glaze_json_schema {
            schema knownPacks{
                .description = "Known content packs' basic info. Used for disabling packs in-game."
            };
            schema packOptions{
                .description = "User-set pack-specified option values."
            };
            schema additionalPackRoots{
                .description = "Additional pack root directories, apart from `/pack`."
            };
            schema additionalPacks{
                .description = "Additional packs."
            };
        };
    } packs;

    JSON_STRUCT Graphics {
        float fov{70.0f};
        float gamma{1.0f};
        //`0` is for unbounded.
        u32 maxFPS{60};
        u32 renderDistance{32};
        u32 windowWidth{1920};
        u32 windowHeight{1080};
        WindowMode windowMode{WindowMode::Windowed};
        bool vsync{true};

        struct glaze_json_schema {
            schema fov{
                .defaultValue = 70.0f,
                .minimum = 30.0f,
                .maximum = 140.0f
            };
            schema gamma{
                .defaultValue = 1.0f,
                .minimum = 0.1f,
                .maximum = 10.0f
            };
            schema maxFPS{
                .description = "`0` is for unbounded FPS.",
                .defaultValue = 60,
                .minimum = 0,
                .maximum = 240
            };
            schema renderDistance{
                .defaultValue = 32,
                .minimum = 1,
                .maximum = 256
            };
            schema windowWidth{
                .defaultValue = 1920,
                .minimum = 1
            };
            schema windowHeight{
                .defaultValue = 1080,
                .minimum = 1
            };
            schema windowMode{
                .defaultValue = "windowed",
                .enumeration = vector<string_view>{"windowed", "fullscreen"},
            };
            schema vsync{
                .defaultValue = true
            };
        };
    } graphics;

    JSON_STRUCT Simulation {
        u32 simulationDistance{16};
        u32 maxPlanckTimePerSec{20};

        struct glaze_json_schema {
            schema simulationDistance{
                .defaultValue = 16,
                .minimum = 1,
                .maximum = 1024
            };
            schema maxPlanckTimePerSec{
                .defaultValue = 20,
                .minimum = 1,
                .maximum = 1024
            };
        };
    } simulation;

    JSON_STRUCT Input {
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

        struct glaze_json_schema {
            schema mouseSensitivity{
                .defaultValue = 1.0f,
                .minimum = 0.1f,
                .maximum = 5.0f
            };
            schema scrollSensitivity{
                .defaultValue = 1.0f,
                .minimum = 0.1f,
                .maximum = 5.0f
            };
            schema repeatTapGap{
                .description = "Time threshold in milliseconds for registering a `KeyPress` event with more than `1` tap count.",
                .defaultValue = 100,
                .minimum = 1,
                .maximum = 1000
            };
            schema comboMinTTL{
                .description = "Time threshold in milliseconds for two consequtive key presses as a key combo.",
                .defaultValue = 100,
                .minimum = 1,
                .maximum = 1000
            };
            schema deadzones{
                .description = "The deadzones for gamepad sticks.",
                .minItems = 8,
                .maxItems = 8
            };
            schema invertMouseX{
                .description = "Whether to invert X screen coordinates for mouses.",
                .defaultValue = false
            };
            schema invertMouseY{
                .description = "Whether to invert Y screen coordinates for mouses.",
                .defaultValue = false
            };
            schema swapSticks{
                .description = "Whether to swap left and right sticks.",
                .defaultValue = false
            };
            schema keybinds{
                .minItems = 0
            };
            schema gamepadBinds{
                .minItems = 0
            };
            schema mouseMoveBinds{
                .minItems = 0
            };
            schema scrollBinds{
                .minItems = 0
            };
            schema stickBinds{
                .minItems = 0
            };
        };
    } input;

    JSON_STRUCT Debug {
        Logger::LoggingMode logging{Logger::LoggingMode::File};
        //I don't know if this is a good idea but let's just try and see.
        u32 maxMainThreadTasksPerFrame{128};
        u32 maxMainThreadLoopTimeUs{1000};

        struct glaze_json_schema {
            schema logging{
                .description = "Where to log to. `file`: Log to {wd}/logs/*.log, `stdout`: Log to standard output, `separate`: Log to a separate console window.",
                .defaultValue = "file",
                .enumeration = vector<string_view>{"file", "stdout", "separate"},
            };
            schema maxMainThreadTasksPerFrame{
                .defaultValue = 128
            };
            schema maxMainThreadLoopTimeUs{
                .defaultValue = 1000
            };
        };
    } debug;

    JSON_STRUCT Misc {
        string windowTitleBase{"CherryGrove"};
        bool checkEngineUpdates{true};

        struct glaze_json_schema {
            schema windowTitleBase{
                .description = "The window title (if applicable)",
                .defaultValue = "CherryGrove"
            };
            schema checkEngineUpdates{
                .defaultValue = true
            };
        };
    } misc;

    };
}