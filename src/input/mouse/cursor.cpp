#include <glfw/glfw3.h>
#include <iostream>
#include "cursor.hpp"

using std::cout, std::endl;

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	cout << xpos << " " << ypos << endl;
}