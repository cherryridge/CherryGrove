#include <glfw/glfw3.h>
#include <iostream>
#include <map>

#include "keyboard.h"
#include "keydesc_win.h"

using std::string, std::cout, std::endl, std::map;

void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto p = keydesc_win.find(key);
	string keyStatus = action == GLFW_RELEASE ? "Release " : action == GLFW_REPEAT ? "Repeat " : "Press down ";
	if (p != keydesc_win.begin()) cout << keyStatus << (*p).second << endl;
	else cout << "unknown" << endl;
}