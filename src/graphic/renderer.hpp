#pragma once
#include <bgfx/bgfx.h>
#include <glfw/glfw3.h>
#include <imgui.h>
#include <iostream>
#include <vector>

using std::vector;

class Renderer {
public:
	Renderer(unsigned int width, unsigned int height, const char* title, const char* iconFilePath = nullptr);
	~Renderer();

	//Common use
public:
	bool isAlive();
	void close();
private:
	static unsigned int instanceCount;
	bool windowAlive;

	//GLFW
public:
	GLFWwindow* getWindow();
	unsigned int getWidth();
	void setWidth(int width);
	unsigned int getHeight();
	void setHeight(int height);
	float getWindowScale();
	double getAspectRatio();
	void setAspectRatio(int widthRatio, int heightRatio);
	const char* getTitle();
	void setTitle(const char* newTitle);
	//Not going to implement callback priority unless necessary.
	void addKeyCB(GLFWkeyfun func/*, int priority*/);
	void removeKeyCB(GLFWkeyfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<GLFWkeyfun> keyCBs;
	void addCharCB(GLFWcharfun func/*, int priority*/);
	void removeCharCB(GLFWcharfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<GLFWcharfun> charCBs;
	void addCursorPosCB(GLFWcursorposfun func);
	void removeCursorPosCB(GLFWcursorposfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<GLFWcursorposfun> cursorposCBs;
	void addCursorEnterCB(GLFWcursorenterfun func);
	void removeCursorEnterCB(GLFWcursorenterfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<GLFWcursorenterfun> cursorenterCBs;
	void addMouseButtonCB(GLFWmousebuttonfun func);
	void removeMouseButtonCB(GLFWmousebuttonfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<GLFWmousebuttonfun> mousebuttonCBs;
	void addScrollCB(GLFWscrollfun func);
	void removeScrollCB(GLFWscrollfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<GLFWscrollfun> scrollCBs;
	//void addJoystickCB(GLFWjoystickfun func);
	//void removeJoystickCB(GLFWjoystickfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	//vector<GLFWjoystickfun> joystickCBs;

	void addDropCB(GLFWdropfun func);
	void removeDropCB(GLFWdropfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<GLFWdropfun> dropCBs;
	void addWindowFocusCB(GLFWwindowfocusfun func);
	void removeWindowFocusCB(GLFWwindowfocusfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<GLFWwindowfocusfun> windowFocusCBs;
	void addWindowSizeCB(GLFWwindowsizefun func);
	void removeWindowSizeCB(GLFWwindowsizefun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	vector<GLFWwindowsizefun> windowSizeCBs;
	//Global
	static void addMonitorCB(GLFWmonitorfun func);
	static void removeMonitorCB(GLFWmonitorfun func);
	//INTERNAL USE ONLY. DO NOT MODIFY!
	static vector<GLFWmonitorfun> monitorCBs;

private:
	GLFWwindow* window;
	//Will change someday soon :)
	static HWND getNativeHandle(GLFWwindow* window);
	static void proxyKeyCB(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void proxyCharCB(GLFWwindow* window, unsigned int codepoint);
	static void proxyCursorPosCB(GLFWwindow* window, double xpos, double ypos);
	static void proxyCursorEnterCB(GLFWwindow* window, int entered);
	static void proxyMouseButtonCB(GLFWwindow* window, int button, int action, int mods);
	static void proxyScrollCB(GLFWwindow* window, double xoffset, double yoffset);
	//static void proxyJoystickCB(int jid, int event);

	static void proxyDropCB(GLFWwindow* window, int count, const char** paths);
	static void proxyWindowFocusCB(GLFWwindow* window, int focused);
	static void proxyWindowSizeCB(GLFWwindow* window, int width, int height);
	static void proxyMonitorCB(GLFWmonitor* monitor, int event);

	//ImGui
public:
	ImGuiContext* getGuiContext();
	void startGuiFrame();
	void submitGuiFrame();
private:
	ImGuiContext* context;
};