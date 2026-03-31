#pragma once
#include <string>
#include <boost/uuid/uuid.hpp>
#include <glaze/glaze.hpp>

#include "../input/boolInput/KeyCombo.hpp"

namespace Settings {
    typedef uint8_t u8;
    typedef uint32_t u32;
    using std::string, boost::uuids::uuid, InputHandler::BoolInput::KeyCombo;

    struct KnownPack {
        uuid id;
        u32 version;
        bool disabled;
    };

    enum struct WindowMode : u8 { Windowed, Fullscreen };

    struct Keybind {
        string nameSpace, identifier;
        KeyCombo combo;
    };

    struct OtherBind {
        string nameSpace, identifier;
        bool disabled;
    };
}

template <>
struct glz::meta<Settings::WindowMode> {
    using T = Settings::WindowMode;
    static constexpr auto value = glz::enumerate(
        "windowed", T::Windowed,
        "fullscreen", T::Fullscreen
    );
};