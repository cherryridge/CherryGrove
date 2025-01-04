#pragma once
#include <bgfx/bgfx.h>
#include <glfw/glfw3.h>
#include <imgui.h>
#include <Windows.h>
#include <memory>

#include "../gui/InputHandler.hpp"

typedef uint16_t u16;
typedef uint32_t u32;

using std::unique_ptr;

class Window {

public:
	Window(unsigned int width, unsigned int height, const char* title);
	~Window();

	//Common use
public:
	unsigned int getInstanceId() const;
	void update() const;
	bool isAlive();
	void close();
private:
	static unsigned int instanceCount;
	unsigned int instanceId;
	bool windowAlive;

	//GLFW
public:
	GLFWwindow* getWindow() const;
	void setIcon(const char* filePath);
	unsigned int getWidth() const;
	void setWidth(int width);
	unsigned int getHeight() const;
	void setHeight(int height);
	float getWindowScale() const;
	double getAspectRatio() const;
	void setAspectRatio(int widthRatio, int heightRatio);
	const char* getTitle() const;
	void setTitle(const char* newTitle);
	InputHandler* getInput() const;

private:
	GLFWwindow* window;
	unique_ptr<InputHandler> input;
	//Will change someday soon :)
	HWND getNativeHandle() const;

	//ImGui
public:
	ImGuiContext* getGuiContext() const;
	void startGuiFrame();
	void submitGuiFrame();
private:
	ImGuiContext* context;

	//bgfx
public:
	const char* getRenderBackend() const;
	void addDrawcall(u16 viewId, u32 shaderId);
	void startFrame();
	void submitFrame();

};