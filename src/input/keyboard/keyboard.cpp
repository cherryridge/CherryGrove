#include <glfw/glfw3.h>
#include <iostream>

#include "../../debug/Logger.hpp"
#include "keyboard.hpp"
#include "keydesc_win.hpp"

using std::string, Logger::lout, std::endl;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto p = keydesc_win.find(key);
	string keyStatus = action == GLFW_RELEASE ? "Release " : action == GLFW_REPEAT ? "Repeat " : "Press down ";
	if (p != keydesc_win.end()) {
		//lout << keyStatus << (*p).second << " " << endl;
		//if (glfwGetKeyName(key, scancode) != nullptr) lout << glfwGetKeyName(key, scancode) << endl;
		//else lout << "unknown key" << endl;
	}
	else lout << "unknown" << endl;
}