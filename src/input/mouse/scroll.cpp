#include <glfw/glfw3.h>
#include <iostream>
#include "scroll.hpp"

using std::cout, std::endl;

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	cout << xoffset << " " << yoffset << endl;
}