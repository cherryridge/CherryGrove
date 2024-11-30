#include <glfw/glfw3.h>
#include <iostream>

#include "mouse.hpp"
#include "mousedesc_win.hpp"

using std::string, std::cout, std::endl, std::map;

void mouseCallback(GLFWwindow* window, int button, int action, int mods){
	auto p = mousedesc_win.find(button);
	string buttonStatus = action == GLFW_RELEASE ? "Release " : action == GLFW_REPEAT ? "Repeat " : "Press down ";
	if (p != mousedesc_win.end()) cout << buttonStatus << (*p).second << endl;
	else cout << "unknown" << endl;
}