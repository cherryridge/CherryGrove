#define NOMINMAX
#include <windows.h>
#include <bgfx/bgfx.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#include <iostream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <bx/string.h>
#include <bx/readerwriter.h>
#include <bx/timer.h>
#include <bx/file.h>
#include <bx/math.h>
#include <stdlib.h>
#include <string.h>
#include <nuklear/nuklear.h>
#include <cstdlib>

#include "logger/logger.hpp"
#include "graphic/shader/shader.hpp"
#include "js/JSEngine.hpp"
#include "input/keyboard/keyboard.hpp"
#include "input/mouse/mouse.hpp"
#include "input/mouse/scroll.hpp"
#include "input/mouse/cursor.hpp"

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920
#define ICONHW 256

using std::cout, std::endl;

//Whether the program is running with a console window.
#ifdef CG_CONSOLE
int main(int argc, char* argv[]) {
	char** _argv = argv;
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	char** _argv = __argv;
	cout << "Setting up logger..." << endl;
	Logger logger;
#endif
#ifdef _DEBUG
	//Hacks for auto compiling shaders every time the program starts, while Visual Studio build events does not trigger when nothing included in the project has changed since last build.
	//Sadly only for Visual Studio x64 debugging.
	cout << "Compiling shaders in Visual Studio Debug..." << endl;
	SetCurrentDirectoryA("scripts");
	system("compile_shaders.bat");
	string path = _argv[0];
	string solutionRootDir = path.substr(0, path.find("\\build\\x64\\Debug"));
	SetCurrentDirectoryA(solutionRootDir.c_str());
#endif
	cout << "Running at " << _argv[0] << endl;
	cout << "Setting up GLFW window..." << endl;
	if (!glfwInit()) {
		cout << "GLFW window failed to set up!" << endl;
		return -1;
	}
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CherryGrove", NULL, NULL);
	if (!window) {
		glfwTerminate();
		cout << "Setting up GLFW window failed!" << endl;
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	cout << "Loading window icon..." << endl;
	int iconHW = ICONHW, channels = 4;
	unsigned char* iconData = stbi_load("assets/icons/CherryGrove-trs-256.png", &iconHW, &iconHW, &channels, 4);
	if (iconData) {
		GLFWimage icon;
		icon.height = ICONHW;
		icon.width = ICONHW;
		icon.pixels = iconData;
		glfwSetWindowIcon(window, 1, &icon);
	}
	else cout << "Load window icon data failed!" << endl;

	JSEngine engine(_argv[0]);

	cout << "Setting up BGFX..." << endl;
	bgfx::Init config;
	bgfx::PlatformData pdata;
	pdata.nwh = glfwGetWin32Window(window);
	//makes bgfx autoselect its rendering backend.
	config.type = bgfx::RendererType::Count;
	//Control for switching to OpenGL temporaily.
	//config.type = bgfx::RendererType::OpenGL;
	config.resolution.width = WINDOW_WIDTH;
	config.resolution.height = WINDOW_HEIGHT;
	config.resolution.reset = BGFX_RESET_VSYNC;
	config.platformData = pdata;
	bgfx::init(config);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	bgfx::setDebug(BGFX_DEBUG_TEXT);
	cout << "Used rendering backend: " << bgfx::getRendererName(bgfx::getRendererType()) << endl;
	cout << "---------------------------" << endl;

	struct PosColorVertex {
		float x;
		float y;
		float z;
		uint32_t abgr;
	};
	static const PosColorVertex cubeVertices[] = {
		{  0.5f,  0.5f,  1.0f },
		{ -0.5f, -0.5f,  1.0f },
		{  0.5f, -0.5f,  1.0f },
		{ -0.5f,  0.5f,  1.0f },
		{ -0.3f,  0.2f,  1.0f },
	};
	static const uint16_t cubeTriList[] = {
		0, 1, 2, 3, 1, 0, 3, 4, 0
	};
	bgfx::VertexLayout layout;
	layout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, true).end();
	bgfx::VertexBufferHandle vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), layout);
	bgfx::IndexBufferHandle vertexIndexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));
	Shader shader("test.vert.bin", "test.frag.bin");
	
	unsigned int counter = 0;
	while (!glfwWindowShouldClose(window)) {
		bgfx::setVertexBuffer(0, vertexBuffer);
		bgfx::setIndexBuffer(vertexIndexBuffer);
		bgfx::submit(0, shader.program);
		bgfx::touch(0);
		bgfx::frame();
		glfwPollEvents();
		counter++;
		if (counter % 100 == 0) cout << "Rendered " << counter << " frames" << endl;
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	bgfx::destroy(vertexBuffer);
	bgfx::shutdown();
	return 0;
}