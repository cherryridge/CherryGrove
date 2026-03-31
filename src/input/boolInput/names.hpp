#pragma once
#include <array>
#include <string>

#include "biid.hpp"

namespace InputHandler::BoolInput {
    typedef uint8_t u8;
    using std::array, std::string;

    namespace detail {
        inline constexpr array<const char*, BIID_COUNT> fallbackCodeNames = {
            "LeftBtn",
            "MiddleBtn",
            "RightBtn",
            "Extra1Btn",
            "Extra2Btn",
            "A",
            "B",
            "C",
            "D",
            "E",
            "F",
            "G",
            "H",
            "I",
            "J",
            "K",
            "L",
            "M",
            "N",
            "O",
            "P",
            "Q",
            "R",
            "S",
            "T",
            "U",
            "V",
            "W",
            "X",
            "Y",
            "Z",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "0",
            "Return",
            "Escape",
            "Backspace",
            "Tab",
            "Space",
            "-",
            "=",
            "[",
            "]",
            "\\",
            "NonUS #",
            ";",
            "'",
            "`",
            ",",
            ".",
            "/",
            "CapsLock",
            "F1",
            "F2",
            "F3",
            "F4",
            "F5",
            "F6",
            "F7",
            "F8",
            "F9",
            "F10",
            "F11",
            "F12",
            "PrtScrn",
            "ScrollLock",
            "Pause",
            "Insert",
            "Home",
            "PageUp",
            "Delete",
            "End",
            "PageDown",
            "→",
            "←",
            "↓",
            "↑",
            "NumLock",
            "KeyPad /",
            "KeyPad *",
            "KeyPad -",
            "KeyPad +",
            "KeyPadEntr",
            "KeyPad 1",
            "KeyPad 2",
            "KeyPad 3",
            "KeyPad 4",
            "KeyPad 5",
            "KeyPad 6",
            "KeyPad 7",
            "KeyPad 8",
            "KeyPad 9",
            "KeyPad 0",
            "KeyPad .",
            "NonUS \\",
            "F13",
            "F14",
            "F15",
            "F16",
            "F17",
            "F18",
            "F19",
            "F20",
            "F21",
            "F22",
            "F23",
            "F24",
            "Menu",
            "Mute",
            "VolumeUp",
            "VolumeDown",
            "LeftCtrl",
            "LeftShift",
            "LeftAlt",
            "LeftGUI",
            "RightCtrl",
            "RightShift",
            "RightAlt",
            "RightGUI",
            "MediaPlay",
            "MediaPause",
            "MediaNxtTr",
            "MediaPrvTr",
            "MediaStop",
            "MediaPlPs",
            "AC Home",
            "AC Back",
            "AC Forward",
            "AC Refresh",
            "SouthBtn",
            "EastBtn",
            "WestBtn",
            "NorthBtn",
            "BackBtn",
            "GuideBtn",
            "StartBtn",
            "LStickBtn",
            "RStickBtn",
            "LShldBtn",
            "RShldBtn",
            "DPad Up",
            "DPad Down",
            "DPad Left",
            "DPad Right",
            "Misc1Btn",
            "RPdl1Btn",
            "LPdl1Btn",
            "RPdl2Btn",
            "LPdl2Btn",
            "Touchpad",
            "Misc2Btn",
            "Misc3Btn",
            "Misc4Btn",
            "Misc5Btn",
            "Misc6Btn",
        };
    }

    [[nodiscard]] inline const char* getBoolInputName(BoolInputID biid) noexcept {
        if (biid >= BIID_COUNT) {
        #if CG_DEBUG
            lerr << "[InputHandler] Out-of-bound BIID in getBoolInputName: " << biid << endl;
        #endif
            return "InvalidInput";
        }
        switch (BIIDToSource(biid)) {
            case BIInputSource::Mouse: return detail::fallbackCodeNames[biid];
            case BIInputSource::Keyboard: {
                const SDL_Scancode rawCode = static_cast<SDL_Scancode>(BIIDToRawCode(biid));
                const auto resultFromScanCode = SDL_GetScancodeName(rawCode);
                if (string(resultFromScanCode) != "") return resultFromScanCode;
                const auto resultFromKey = SDL_GetKeyName(rawCode);
                if (string(resultFromKey) != "") return resultFromKey;
                return detail::fallbackCodeNames[biid];
            }
            case BIInputSource::Gamepad: {
                const auto result = SDL_GetGamepadStringForButton(static_cast<SDL_GamepadButton>(BIIDToRawCode(biid)));
                if (result == nullptr) return "UnknownGamepadButton";
                if (string(result) == "") return detail::fallbackCodeNames[biid];
                return result;
            }
            default:
            #if CG_DEBUG
                lerr << "[InputHandler] Unrecognized BIInputSource in getBoolInputName: " << static_cast<u8>(BIIDToSource(biid)) << endl;
            #endif
                return "InvalidInput";
        }
    }
}