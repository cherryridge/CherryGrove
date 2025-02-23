#pragma once
#include <atomic>
#include <glfw/glfw3.h>

namespace MainWindow {
	typedef int32_t i32;
	typedef uint32_t u32;

	extern GLFWwindow* window;

	void initGlfw(u32 width, u32 height, const char* title);
	//Must be called after `initGlfw`.
	void initInputHandler();
	void update();
	void close();

	u32 getWidth();
	u32 getHeight();
	float getAspectRatio();
	void setWidth(i32 width);
	void setHeight(i32 height);

	void loadIcon(const char* filePath);
	void setIcon();
}