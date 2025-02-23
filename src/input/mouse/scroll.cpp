#include <glfw/glfw3.h>

#include "../../debug/debug.hpp"
#include "scroll.hpp"

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	lout << xoffset << " " << yoffset << endl;
}