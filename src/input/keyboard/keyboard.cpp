#include <glfw/glfw3.h>
#include <iostream>

#include "keyboard.hpp"
#include "keydesc_win.hpp"

using std::string, std::cout, std::endl;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto p = keydesc_win.find(key);
	string keyStatus = action == GLFW_RELEASE ? "Release " : action == GLFW_REPEAT ? "Repeat " : "Press down ";
	if (p != keydesc_win.end()) {
		//cout << keyStatus << (*p).second << " " << endl;
		//if (glfwGetKeyName(key, scancode) != nullptr) cout << glfwGetKeyName(key, scancode) << endl;
		//else cout << "unknown key" << endl;
	}
	else cout << "unknown" << endl;
}