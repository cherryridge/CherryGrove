#pragma once
#include <limits>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"
#include "../../util/BijectorArray.hpp"

namespace InputHandler::BoolInput {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    using std::numeric_limits, Util::BijectorArray;
    using BoolInputID = u16;

    //THE SCHEMA OF SOURCED CODE (u16):
    //[I][I][I][I][-][-][A][A] [A][A][A][A][A][A][A][A]
    //         ^^^                                  ^^^
    //         BIInputSource <<= 12                 Actual Scancode (This bitch's sometimes bigger than u8 so we have to hack)
    //important: SOURCED CODE IS NOT BoolInputID!! BoolInputID is continuous and forms a bijection with all sourced codes.

    enum struct BIInputSource : u8 { Mouse, Keyboard, Gamepad, Count };

    inline constexpr u8 BIID_COUNT = 162;

    //NB: `INVALID_BIID` at 0 is gone! yay!!!!
    inline constexpr BoolInputID
        BIID_MOUSE_START    = 0  , BIID_MOUSE_END    = 4  ,
        BIID_KEYBOARD_START = 5  , BIID_KEYBOARD_END = 135,
        BIID_GAMEPAD_START  = 136, BIID_GAMEPAD_END  = 161,
        INVALID_BIID = numeric_limits<BoolInputID>::max();

    inline constexpr u32 INVALID_RAWCODE = numeric_limits<u32>::max();

    namespace detail {
        inline constexpr u16 RAWCODE_MASK = 0x0FFF;

        //It's an attitude pal!
        #define MOUSE(scancode) (static_cast<u16>(scancode) | static_cast<u16>(BIInputSource::Mouse) << 12)
        #define KEYBOARD(scancode) (static_cast<u16>(scancode) | static_cast<u16>(BIInputSource::Keyboard) << 12)
        #define GAMEPAD(scancode) (static_cast<u16>(scancode) | static_cast<u16>(BIInputSource::Gamepad) << 12)

        //Typical strategy of buying time with space.
        inline constexpr BijectorArray<BoolInputID, BIID_COUNT, 8218> BIIDtoSourcedCode = {{
            MOUSE(SDL_BUTTON_LEFT),
            MOUSE(SDL_BUTTON_MIDDLE),
            MOUSE(SDL_BUTTON_RIGHT),
            MOUSE(SDL_BUTTON_X1),
            MOUSE(SDL_BUTTON_X2),
            KEYBOARD(SDL_SCANCODE_A),
            KEYBOARD(SDL_SCANCODE_B),
            KEYBOARD(SDL_SCANCODE_C),
            KEYBOARD(SDL_SCANCODE_D),
            KEYBOARD(SDL_SCANCODE_E),
            KEYBOARD(SDL_SCANCODE_F),
            KEYBOARD(SDL_SCANCODE_G),
            KEYBOARD(SDL_SCANCODE_H),
            KEYBOARD(SDL_SCANCODE_I),
            KEYBOARD(SDL_SCANCODE_J),
            KEYBOARD(SDL_SCANCODE_K),
            KEYBOARD(SDL_SCANCODE_L),
            KEYBOARD(SDL_SCANCODE_M),
            KEYBOARD(SDL_SCANCODE_N),
            KEYBOARD(SDL_SCANCODE_O),
            KEYBOARD(SDL_SCANCODE_P),
            KEYBOARD(SDL_SCANCODE_Q),
            KEYBOARD(SDL_SCANCODE_R),
            KEYBOARD(SDL_SCANCODE_S),
            KEYBOARD(SDL_SCANCODE_T),
            KEYBOARD(SDL_SCANCODE_U),
            KEYBOARD(SDL_SCANCODE_V),
            KEYBOARD(SDL_SCANCODE_W),
            KEYBOARD(SDL_SCANCODE_X),
            KEYBOARD(SDL_SCANCODE_Y),
            KEYBOARD(SDL_SCANCODE_Z),
            KEYBOARD(SDL_SCANCODE_1),
            KEYBOARD(SDL_SCANCODE_2),
            KEYBOARD(SDL_SCANCODE_3),
            KEYBOARD(SDL_SCANCODE_4),
            KEYBOARD(SDL_SCANCODE_5),
            KEYBOARD(SDL_SCANCODE_6),
            KEYBOARD(SDL_SCANCODE_7),
            KEYBOARD(SDL_SCANCODE_8),
            KEYBOARD(SDL_SCANCODE_9),
            KEYBOARD(SDL_SCANCODE_0),
            KEYBOARD(SDL_SCANCODE_RETURN),
            KEYBOARD(SDL_SCANCODE_ESCAPE),
            KEYBOARD(SDL_SCANCODE_BACKSPACE),
            KEYBOARD(SDL_SCANCODE_TAB),
            KEYBOARD(SDL_SCANCODE_SPACE),
            KEYBOARD(SDL_SCANCODE_MINUS),
            KEYBOARD(SDL_SCANCODE_EQUALS),
            KEYBOARD(SDL_SCANCODE_LEFTBRACKET),
            KEYBOARD(SDL_SCANCODE_RIGHTBRACKET),
            KEYBOARD(SDL_SCANCODE_BACKSLASH),
            KEYBOARD(SDL_SCANCODE_NONUSHASH),
            KEYBOARD(SDL_SCANCODE_SEMICOLON),
            KEYBOARD(SDL_SCANCODE_APOSTROPHE),
            KEYBOARD(SDL_SCANCODE_GRAVE),
            KEYBOARD(SDL_SCANCODE_COMMA),
            KEYBOARD(SDL_SCANCODE_PERIOD),
            KEYBOARD(SDL_SCANCODE_SLASH),
            KEYBOARD(SDL_SCANCODE_CAPSLOCK),
            KEYBOARD(SDL_SCANCODE_F1),
            KEYBOARD(SDL_SCANCODE_F2),
            KEYBOARD(SDL_SCANCODE_F3),
            KEYBOARD(SDL_SCANCODE_F4),
            KEYBOARD(SDL_SCANCODE_F5),
            KEYBOARD(SDL_SCANCODE_F6),
            KEYBOARD(SDL_SCANCODE_F7),
            KEYBOARD(SDL_SCANCODE_F8),
            KEYBOARD(SDL_SCANCODE_F9),
            KEYBOARD(SDL_SCANCODE_F10),
            KEYBOARD(SDL_SCANCODE_F11),
            KEYBOARD(SDL_SCANCODE_F12),
            KEYBOARD(SDL_SCANCODE_PRINTSCREEN),
            KEYBOARD(SDL_SCANCODE_SCROLLLOCK),
            KEYBOARD(SDL_SCANCODE_PAUSE),
            KEYBOARD(SDL_SCANCODE_INSERT),
            KEYBOARD(SDL_SCANCODE_HOME),
            KEYBOARD(SDL_SCANCODE_PAGEUP),
            KEYBOARD(SDL_SCANCODE_DELETE),
            KEYBOARD(SDL_SCANCODE_END),
            KEYBOARD(SDL_SCANCODE_PAGEDOWN),
            KEYBOARD(SDL_SCANCODE_RIGHT),
            KEYBOARD(SDL_SCANCODE_LEFT),
            KEYBOARD(SDL_SCANCODE_DOWN),
            KEYBOARD(SDL_SCANCODE_UP),
            KEYBOARD(SDL_SCANCODE_NUMLOCKCLEAR),
            KEYBOARD(SDL_SCANCODE_KP_DIVIDE),
            KEYBOARD(SDL_SCANCODE_KP_MULTIPLY),
            KEYBOARD(SDL_SCANCODE_KP_MINUS),
            KEYBOARD(SDL_SCANCODE_KP_PLUS),
            KEYBOARD(SDL_SCANCODE_KP_ENTER),
            KEYBOARD(SDL_SCANCODE_KP_1),
            KEYBOARD(SDL_SCANCODE_KP_2),
            KEYBOARD(SDL_SCANCODE_KP_3),
            KEYBOARD(SDL_SCANCODE_KP_4),
            KEYBOARD(SDL_SCANCODE_KP_5),
            KEYBOARD(SDL_SCANCODE_KP_6),
            KEYBOARD(SDL_SCANCODE_KP_7),
            KEYBOARD(SDL_SCANCODE_KP_8),
            KEYBOARD(SDL_SCANCODE_KP_9),
            KEYBOARD(SDL_SCANCODE_KP_0),
            KEYBOARD(SDL_SCANCODE_KP_PERIOD),
            KEYBOARD(SDL_SCANCODE_NONUSBACKSLASH),
            KEYBOARD(SDL_SCANCODE_F13),
            KEYBOARD(SDL_SCANCODE_F14),
            KEYBOARD(SDL_SCANCODE_F15),
            KEYBOARD(SDL_SCANCODE_F16),
            KEYBOARD(SDL_SCANCODE_F17),
            KEYBOARD(SDL_SCANCODE_F18),
            KEYBOARD(SDL_SCANCODE_F19),
            KEYBOARD(SDL_SCANCODE_F20),
            KEYBOARD(SDL_SCANCODE_F21),
            KEYBOARD(SDL_SCANCODE_F22),
            KEYBOARD(SDL_SCANCODE_F23),
            KEYBOARD(SDL_SCANCODE_F24),
            KEYBOARD(SDL_SCANCODE_MENU),
            KEYBOARD(SDL_SCANCODE_MUTE),
            KEYBOARD(SDL_SCANCODE_VOLUMEUP),
            KEYBOARD(SDL_SCANCODE_VOLUMEDOWN),
            KEYBOARD(SDL_SCANCODE_LCTRL),
            KEYBOARD(SDL_SCANCODE_LSHIFT),
            KEYBOARD(SDL_SCANCODE_LALT),
            KEYBOARD(SDL_SCANCODE_LGUI),
            KEYBOARD(SDL_SCANCODE_RCTRL),
            KEYBOARD(SDL_SCANCODE_RSHIFT),
            KEYBOARD(SDL_SCANCODE_RALT),
            KEYBOARD(SDL_SCANCODE_RGUI),
            KEYBOARD(SDL_SCANCODE_MEDIA_PLAY),
            KEYBOARD(SDL_SCANCODE_MEDIA_PAUSE),
            KEYBOARD(SDL_SCANCODE_MEDIA_NEXT_TRACK),
            KEYBOARD(SDL_SCANCODE_MEDIA_PREVIOUS_TRACK),
            KEYBOARD(SDL_SCANCODE_MEDIA_STOP),
            KEYBOARD(SDL_SCANCODE_MEDIA_PLAY_PAUSE),
            KEYBOARD(SDL_SCANCODE_AC_HOME),
            KEYBOARD(SDL_SCANCODE_AC_BACK),
            KEYBOARD(SDL_SCANCODE_AC_FORWARD),
            KEYBOARD(SDL_SCANCODE_AC_REFRESH),
            GAMEPAD(SDL_GAMEPAD_BUTTON_SOUTH),
            GAMEPAD(SDL_GAMEPAD_BUTTON_EAST),
            GAMEPAD(SDL_GAMEPAD_BUTTON_WEST),
            GAMEPAD(SDL_GAMEPAD_BUTTON_NORTH),
            GAMEPAD(SDL_GAMEPAD_BUTTON_BACK),
            GAMEPAD(SDL_GAMEPAD_BUTTON_GUIDE),
            GAMEPAD(SDL_GAMEPAD_BUTTON_START),
            GAMEPAD(SDL_GAMEPAD_BUTTON_LEFT_STICK),
            GAMEPAD(SDL_GAMEPAD_BUTTON_RIGHT_STICK),
            GAMEPAD(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER),
            GAMEPAD(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER),
            GAMEPAD(SDL_GAMEPAD_BUTTON_DPAD_UP),
            GAMEPAD(SDL_GAMEPAD_BUTTON_DPAD_DOWN),
            GAMEPAD(SDL_GAMEPAD_BUTTON_DPAD_LEFT),
            GAMEPAD(SDL_GAMEPAD_BUTTON_DPAD_RIGHT),
            GAMEPAD(SDL_GAMEPAD_BUTTON_MISC1),
            GAMEPAD(SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1),
            GAMEPAD(SDL_GAMEPAD_BUTTON_LEFT_PADDLE1),
            GAMEPAD(SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2),
            GAMEPAD(SDL_GAMEPAD_BUTTON_LEFT_PADDLE2),
            GAMEPAD(SDL_GAMEPAD_BUTTON_TOUCHPAD),
            GAMEPAD(SDL_GAMEPAD_BUTTON_MISC2),
            GAMEPAD(SDL_GAMEPAD_BUTTON_MISC3),
            GAMEPAD(SDL_GAMEPAD_BUTTON_MISC4),
            GAMEPAD(SDL_GAMEPAD_BUTTON_MISC5),
            GAMEPAD(SDL_GAMEPAD_BUTTON_MISC6),
        }};
    }

    [[nodiscard]] inline constexpr BoolInputID getBIID(BIInputSource source, u32 rawCode) noexcept {
        switch (source) {
            case BIInputSource::Mouse:    return detail::BIIDtoSourcedCode.from(MOUSE(rawCode));
            case BIInputSource::Keyboard: return detail::BIIDtoSourcedCode.from(KEYBOARD(rawCode));
            case BIInputSource::Gamepad:  return detail::BIIDtoSourcedCode.from(GAMEPAD(rawCode));
            default:                      return INVALID_BIID;
        }
    }

    [[nodiscard]] inline constexpr u32 BIIDToRawCode(BoolInputID biid) noexcept {
        if (biid == INVALID_BIID) {
        #if CG_DEBUG
            lerr << "[InputHandler] Passing invalid BIID to BIIDToRawCode!" << endl;
        #endif
            return INVALID_RAWCODE;
        }
        switch (static_cast<BIInputSource>(detail::BIIDtoSourcedCode.to(biid) >> 12)) {
            case BIInputSource::Mouse:
            case BIInputSource::Keyboard:
            case BIInputSource::Gamepad:
                return detail::BIIDtoSourcedCode.to(biid) & detail::RAWCODE_MASK;
            default:
                return INVALID_RAWCODE;
        }
    }

    [[nodiscard]] inline constexpr BIInputSource BIIDToSource(BoolInputID biid) noexcept {
        if (biid == INVALID_BIID) {
        #if CG_DEBUG
            lerr << "[InputHandler] Passing invalid BIID to BIIDToSource!" << endl;
        #endif
            return BIInputSource::Count;
        }
        return static_cast<BIInputSource>(detail::BIIDtoSourcedCode.to(biid) >> 12);
    }
}