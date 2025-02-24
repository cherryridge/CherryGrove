#include <atomic>
#include <cstdint>
#include <thread>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glfw/glfw3.h>

#include "../input/InputHandler.hpp"
#include "../debug/debug.hpp"
#include "../graphic/Renderer.hpp"
#include "../sound/Sound.hpp"
#include "../input/keyboard/keyboard.hpp"
#include "../input/mouse/mouse.hpp"
#include "../input/mouse/scroll.hpp"
#include "../input/mouse/cursor.hpp"
#include "../CherryGrove.hpp"
#include "Guis.hpp"
#include "MainWindow.hpp"

#include <fstream>

namespace MainWindow {
	typedef int32_t i32;
	typedef uint32_t u32;
	using std::thread, std::atomic;

	atomic<bool> iconReady(false);
	GLFWimage icon;
	GLFWwindow* window;

	void initGlfw(u32 width, u32 height, const char* title) {
		if (!glfwInit()) {
			lerr << "[Window] Failed to set up GLFW!" << endl;
			Fatal::exit(Fatal::GLFW_INITIALIZATION_FALILED);
		}
		window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!window) {
			glfwTerminate();
			lerr << "[Window] Failed to create GLFW window!" << endl;
			Fatal::exit(Fatal::GLFW_CREATE_WINDOW_FAILED);
		}
		glfwMakeContextCurrent(window);
	}

	void initInputHandler() {
		InputHandler::init();
		InputHandler::addKeyCB(keyCallback);
		InputHandler::addMouseButtonCB(mouseCallback);
		InputHandler::addScrollCB(scrollCallback);
		InputHandler::addCursorPosCB(cursorPosCallback);
	}

	void update() {
		if (glfwWindowShouldClose(window)) {
			CherryGrove::isCGAlive = false;
			return;
		}
		if (iconReady) { setIcon(); }
		glfwWaitEvents();
	}

	void close() {
		glfwPollEvents();
		glfwSetWindowShouldClose(window, 1);
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	u32 getWidth(){
		i32 width;
		glfwGetWindowSize(window, &width, nullptr);
		return width;
	}

	u32 getHeight(){
		i32 height;
		glfwGetWindowSize(window, nullptr, &height);
		return height;
	}

	float getAspectRatio() {
		i32 width, height;
		glfwGetWindowSize(window, &width, &height);
		return (float)width / height;
	}

	void setWidth(i32 width) { glfwSetWindowSize(window, width, getHeight()); }

	void setHeight(i32 height) { glfwSetWindowSize(window, getWidth(), height); }

	static void s_loadIcon(const char* filePath) {
		stbi_set_flip_vertically_on_load(0);
		i32 iconWidth, iconHeight;
		unsigned char* iconData = stbi_load(filePath, &iconWidth, &iconHeight, nullptr, 4);
		if (iconData) {
			icon.width = iconWidth;
			icon.height = iconHeight;
			icon.pixels = iconData;
			iconReady = true;
		}
		else lerr << "[Window] Load window icon data failed!" << endl;
	}

	void loadIcon(const char* filePath) {
		lout << "Loading window icon from " << filePath << endl;
		//fixme: stbi can't fopen the icon when running in a detached thread.
		//This is not a concurrent stbi_load conflict.
		thread fileReadThread(&s_loadIcon, filePath);
		fileReadThread.detach();
	}

	void setIcon() {
		glfwSetWindowIcon(window, 1, &icon);
		//Prevent setting icon infinitely.
		iconReady = false;
	}
}