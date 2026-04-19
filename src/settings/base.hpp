#pragma once
#include <string>
#include <glaze/glaze.hpp>

#include "../input/boolInput/KeyCombo.hpp"
#include "../util/json/helpers.hpp"
#include "../util/lexical.hpp"

namespace Settings {
    typedef uint8_t u8;
    typedef uint32_t u32;
    using std::string, glz::schema, InputHandler::BoolInput::KeyCombo;

    enum struct WindowMode : u8 { Windowed, Fullscreen };

    JSON_STRUCT Keybind {
        string nameSpace, identifier;
        KeyCombo combo;

        struct glaze_json_schema {
            schema nameSpace{
                .description = "The namespace of the action to bind."
            };
            schema identifier{
                .description = "The identifier of the action to bind."
            };
            schema combo{
                .description = "The key combo that triggers this action."
            };
        };
    };

    JSON_STRUCT OtherBind {
        string nameSpace, identifier;
        bool disabled;

        struct glaze_json_schema {
            schema nameSpace{
                .description = "The namespace of the action to bind."
            };
            schema identifier{
                .description = "The identifier of the action to bind."
            };
            schema disabled{
                .description = "Whether this binding is disabled.",
                .defaultValue = false
            };
        };
    };
}

GLAZE_ENUM_START(Settings::WindowMode)
    GLAZE_ENUM("windowed", Windowed),
    GLAZE_ENUM("fullscreen", Fullscreen)
GLAZE_ENUM_END

GLAZE_STATIC_CONSTRAINT_BEGIN(Settings::Keybind)
    GLAZE_STATIC_CONSTRAINT(nameSpace, Util::isValidIdentifier(nameSpace),
        "`nameSpace` should be a valid conventional identifier."
    ),
    GLAZE_STATIC_CONSTRAINT(identifier, Util::isValidIdentifier(identifier),
        "`identifier` should be a valid conventional identifier."
    )
GLAZE_STATIC_CONSTRAINT_END

GLAZE_STATIC_CONSTRAINT_BEGIN(Settings::OtherBind)
    GLAZE_STATIC_CONSTRAINT(nameSpace, Util::isValidIdentifier(nameSpace),
        "`nameSpace` should be a valid conventional identifier."
    ),
    GLAZE_STATIC_CONSTRAINT(identifier, Util::isValidIdentifier(identifier),
        "`identifier` should be a valid conventional identifier."
    )
GLAZE_STATIC_CONSTRAINT_END