#include <glfw/glfw3.h>
#include <iostream>

#include "../../debug/Logger.hpp"
#include "cursor.hpp"

using Logger::lout, std::endl;

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	//lout << xpos << " " << ypos << endl;
}