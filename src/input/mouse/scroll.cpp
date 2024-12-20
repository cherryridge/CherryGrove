#include <glfw/glfw3.h>
#include <iostream>

#include "../../debug/Logger.hpp"
#include "scroll.hpp"

using Logger::lout, std::endl;

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	lout << xoffset << " " << yoffset << endl;
}