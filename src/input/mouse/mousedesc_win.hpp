#pragma once
#include <glfw/glfw3.h>
#include <unordered_map>
#include <string>

using std::unordered_map, std::string;

static const unordered_map<int, string> mousedesc_win = {
	{ GLFW_MOUSE_BUTTON_1, "LeftBtn"},
	{ GLFW_MOUSE_BUTTON_2, "RightBtn"},
	{ GLFW_MOUSE_BUTTON_3, "MiddleBtn"},
	{ GLFW_MOUSE_BUTTON_4, "Button4"},
	{ GLFW_MOUSE_BUTTON_5, "Button5"},
	{ GLFW_MOUSE_BUTTON_6, "Button6"},
	{ GLFW_MOUSE_BUTTON_7, "Button7"},
	{ GLFW_MOUSE_BUTTON_8, "Button8"},
};