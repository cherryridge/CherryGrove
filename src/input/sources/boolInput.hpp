#pragma once
#include <array>
#include <map>
#include <glfw/glfw3.h>

#include "../inputBase.hpp"

namespace InputHandler::BoolInput {
	typedef int32_t i32;
	//This is a subset of `i32` consisting of GLFW mouse button codes and key codes.
	//However, no safety check is enforced.
	typedef i32 BoolInputID;

	enum BIEventType {
		BIEventStart,
		BIEventPersist,
		BIEventEnd
	};

	enum BIStatus {
		BIInactive,
		//Instant state, will go to `BIRepeat`
		BIPress,
		BIRepeat,
		//Instant state, will go to `BIInactive`
		BIRelease
	};

	struct BISource {
		BoolInputID inputCode;
		const char* name;
	};

	//For events that are not binded to any bool input.
	inline constexpr BoolInputID emptyBISource = 12914;

	struct BIEvent;

	//Reason why we put `BICallback` into `BIEvent` and thus callback functions can technically see/call other input event's callback functions:
	//1. Internal events can see/call other event's callbacks.
	//2. We are able to filter out `BICallback` for V8 and WASM, so pack input events can't see/call them in the future.
	//3. If we don't do this, we will need to construct three new multimaps for callback functions to see all the callback functions binded to this key each tick, which is not a trivial task.
	using BICallback = void(*)(const std::multimap<EventPriority, BIEvent>& events, EventPriority priority, EventFlags flags, BoolInputID triggerId);

	struct BIEvent {
		InputEventInfo info;
		BICallback cb;
		BoolInputID defaultBinding;

		BIEvent(const InputEventInfo& info, BICallback cb, BoolInputID defaultBinding) : info(info), cb(cb), defaultBinding(defaultBinding) {};
	};

	void init();

	void s_keyCB(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);
	void s_mouseButtonCB(GLFWwindow* window, i32 button, i32 action, i32 mods);

	inline constexpr const char* findBoolInputName(BoolInputID inputCode);

	void addBoolInput(BIEventType type, const InputEventInfo& info, BICallback cb, BoolInputID defaultBinding = emptyBISource);
	bool removeBoolInput(BIEventType type, BICallback cb);

	void changeBinding();
	void resetBinding();

	void s_process();

	//Joystick buttons need to be acquired at runtime, so it's not in bool input.
	inline constexpr std::array<BISource, 1 + 8 + 120> boolInputDesc = {
		BISource { emptyBISource, "" },
		//Mouse button
		BISource{ GLFW_MOUSE_BUTTON_1, "LeftBtn" },
		BISource{ GLFW_MOUSE_BUTTON_2, "RightBtn" },
		BISource{ GLFW_MOUSE_BUTTON_3, "MiddleBtn" },
		BISource{ GLFW_MOUSE_BUTTON_4, "Button4" },
		BISource{ GLFW_MOUSE_BUTTON_5, "Button5" },
		BISource{ GLFW_MOUSE_BUTTON_6, "Button6" },
		BISource{ GLFW_MOUSE_BUTTON_7, "Button7" },
		BISource{ GLFW_MOUSE_BUTTON_8, "Button8" },
		//Keyboard
		BISource{ GLFW_KEY_SPACE, "Space" },
		BISource{ GLFW_KEY_APOSTROPHE, "'" },
		BISource{ GLFW_KEY_COMMA, "," },
		BISource{ GLFW_KEY_MINUS, "-" },
		BISource{ GLFW_KEY_PERIOD, "." },
		BISource{ GLFW_KEY_SLASH, "/" },
		BISource{ GLFW_KEY_0, "0" },
		BISource{ GLFW_KEY_1, "1" },
		BISource{ GLFW_KEY_2, "2" },
		BISource{ GLFW_KEY_3, "3" },
		BISource{ GLFW_KEY_4, "4" },
		BISource{ GLFW_KEY_5, "5" },
		BISource{ GLFW_KEY_6, "6" },
		BISource{ GLFW_KEY_7, "7" },
		BISource{ GLFW_KEY_8, "8" },
		BISource{ GLFW_KEY_9, "9" },
		BISource{ GLFW_KEY_SEMICOLON, ";" },
		BISource{ GLFW_KEY_EQUAL, "=" },
		BISource{ GLFW_KEY_A, "A" },
		BISource{ GLFW_KEY_B, "B" },
		BISource{ GLFW_KEY_C, "C" },
		BISource{ GLFW_KEY_D, "D" },
		BISource{ GLFW_KEY_E, "E" },
		BISource{ GLFW_KEY_F, "F" },
		BISource{ GLFW_KEY_G, "G" },
		BISource{ GLFW_KEY_H, "H" },
		BISource{ GLFW_KEY_I, "I" },
		BISource{ GLFW_KEY_J, "J" },
		BISource{ GLFW_KEY_K, "K" },
		BISource{ GLFW_KEY_L, "L" },
		BISource{ GLFW_KEY_M, "M" },
		BISource{ GLFW_KEY_N, "N" },
		BISource{ GLFW_KEY_O, "O" },
		BISource{ GLFW_KEY_P, "P" },
		BISource{ GLFW_KEY_Q, "Q" },
		BISource{ GLFW_KEY_R, "R" },
		BISource{ GLFW_KEY_S, "S" },
		BISource{ GLFW_KEY_T, "T" },
		BISource{ GLFW_KEY_U, "U" },
		BISource{ GLFW_KEY_V, "V" },
		BISource{ GLFW_KEY_W, "W" },
		BISource{ GLFW_KEY_X, "X" },
		BISource{ GLFW_KEY_Y, "Y" },
		BISource{ GLFW_KEY_Z, "Z" },
		BISource{ GLFW_KEY_LEFT_BRACKET, "[" },
		BISource{ GLFW_KEY_BACKSLASH, "\\" },
		BISource{ GLFW_KEY_RIGHT_BRACKET, "]" },
		BISource{ GLFW_KEY_GRAVE_ACCENT, "`" },
		BISource{ GLFW_KEY_WORLD_1, "World1" },
		BISource{ GLFW_KEY_WORLD_2, "World2" },
		BISource{ GLFW_KEY_ESCAPE, "Esc" },
		BISource{ GLFW_KEY_ENTER, "Enter" },
		BISource{ GLFW_KEY_TAB, "Tab" },
		BISource{ GLFW_KEY_BACKSPACE, "Backspace" },
		BISource{ GLFW_KEY_INSERT, "Ins" },
		BISource{ GLFW_KEY_DELETE, "Del" },
		BISource{ GLFW_KEY_RIGHT, "→" },
		BISource{ GLFW_KEY_LEFT, "←" },
		BISource{ GLFW_KEY_DOWN, "↓" },
		BISource{ GLFW_KEY_UP, "↑" },
		BISource{ GLFW_KEY_PAGE_UP, "PgUp" },
		BISource{ GLFW_KEY_PAGE_DOWN, "PgDn" },
		BISource{ GLFW_KEY_HOME, "Home" },
		BISource{ GLFW_KEY_END, "End" },
		BISource{ GLFW_KEY_CAPS_LOCK, "CapsLk" },
		BISource{ GLFW_KEY_SCROLL_LOCK, "ScrLk" },
		BISource{ GLFW_KEY_NUM_LOCK, "NumLk" },
		BISource{ GLFW_KEY_PRINT_SCREEN, "PrtSc" },
		BISource{ GLFW_KEY_PAUSE, "Pause" },
		BISource{ GLFW_KEY_F1, "F1" },
		BISource{ GLFW_KEY_F2, "F2" },
		BISource{ GLFW_KEY_F3, "F3" },
		BISource{ GLFW_KEY_F4, "F4" },
		BISource{ GLFW_KEY_F5, "F5" },
		BISource{ GLFW_KEY_F6, "F6" },
		BISource{ GLFW_KEY_F7, "F7" },
		BISource{ GLFW_KEY_F8, "F8" },
		BISource{ GLFW_KEY_F9, "F9" },
		BISource{ GLFW_KEY_F10, "F10" },
		BISource{ GLFW_KEY_F11, "F11" },
		BISource{ GLFW_KEY_F12, "F12" },
		BISource{ GLFW_KEY_F13, "F13" },
		BISource{ GLFW_KEY_F14, "F14" },
		BISource{ GLFW_KEY_F15, "F15" },
		BISource{ GLFW_KEY_F16, "F16" },
		BISource{ GLFW_KEY_F17, "F17" },
		BISource{ GLFW_KEY_F18, "F18" },
		BISource{ GLFW_KEY_F19, "F19" },
		BISource{ GLFW_KEY_F20, "F20" },
		BISource{ GLFW_KEY_F21, "F21" },
		BISource{ GLFW_KEY_F22, "F22" },
		BISource{ GLFW_KEY_F23, "F23" },
		BISource{ GLFW_KEY_F24, "F24" },
		BISource{ GLFW_KEY_F25, "F25" },
		BISource{ GLFW_KEY_KP_0, "NumPd0" },
		BISource{ GLFW_KEY_KP_1, "NumPd1" },
		BISource{ GLFW_KEY_KP_2, "NumPd2" },
		BISource{ GLFW_KEY_KP_3, "NumPd3" },
		BISource{ GLFW_KEY_KP_4, "NumPd4" },
		BISource{ GLFW_KEY_KP_5, "NumPd5" },
		BISource{ GLFW_KEY_KP_6, "NumPd6" },
		BISource{ GLFW_KEY_KP_7, "NumPd7" },
		BISource{ GLFW_KEY_KP_8, "NumPd8" },
		BISource{ GLFW_KEY_KP_9, "NumPd9" },
		BISource{ GLFW_KEY_KP_DECIMAL, "NumPd." },
		BISource{ GLFW_KEY_KP_DIVIDE, "NumPd/" },
		BISource{ GLFW_KEY_KP_MULTIPLY, "NumPd*" },
		BISource{ GLFW_KEY_KP_SUBTRACT, "NumPd-" },
		BISource{ GLFW_KEY_KP_ADD, "NumPd+" },
		BISource{ GLFW_KEY_KP_ENTER, "NumPdEnter" },
		BISource{ GLFW_KEY_KP_EQUAL, "NumPd=" },
		BISource{ GLFW_KEY_LEFT_SHIFT, "LShift" },
		BISource{ GLFW_KEY_LEFT_CONTROL, "LCtrl" },
		BISource{ GLFW_KEY_LEFT_ALT, "LAlt" },
		BISource{ GLFW_KEY_LEFT_SUPER, "LSuper" },
		BISource{ GLFW_KEY_RIGHT_SHIFT, "RShift" },
		BISource{ GLFW_KEY_RIGHT_CONTROL, "RCtrl" },
		BISource{ GLFW_KEY_RIGHT_ALT, "RAlt" },
		BISource{ GLFW_KEY_RIGHT_SUPER, "RSuper" },
		BISource{ GLFW_KEY_MENU, "Menu" },
	};
}