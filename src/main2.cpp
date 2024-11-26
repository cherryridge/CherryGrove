#define V8_COMPRESS_POINTERS 1
#define V8_31BIT_SMIS_ON_64BIT_ARCH 1
#define V8_ENABLE_SANDBOX 1
#define NOMINMAX
#include <windows.h>
#include <bgfx/bgfx.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#include <iostream>
#include <direct.h>
#include <cstdio>
#include <fstream>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <bx/string.h>
#include <bx/readerwriter.h>
#include <bx/timer.h>
#include <bx/file.h>
#include <bx/math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <v8/libplatform/libplatform.h>
#include <v8/v8-context.h>
#include <v8/v8-initialization.h>
#include <v8/v8-isolate.h>
#include <v8/v8-local-handle.h>
#include <v8/v8-primitive.h>
#include <v8/v8-script.h>

#include "logger/logger.h"
#include "graphic/shader/shader.h"

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920
#define ICONHW 256

#ifdef CG_CONSOLE
int main(int argc, char* argv[]) {
	char** _argv = argv;
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	char** _argv = __argv;
	std::cout << "Setting up logger..." << std::endl;
	Logger logger;
#endif
	std::cout << "Running at " << _argv[0] << std::endl;
	std::cout << "Setting up GLFW window..." << std::endl;
	if (!glfwInit()) {
		std::cout << "GLFW window failed to set up!" << std::endl;
		return -1;
	}
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cherry Grove", NULL, NULL);
	if (!window) {
		glfwTerminate();
		std::cout << "Setting up GLFW window failed!" << std::endl;
		return -1;
	}
	std::cout << "Loading window icon..." << std::endl;
	int iconHW = ICONHW, channels = 4;
	unsigned char* iconData = stbi_load("assets/icons/CherryGrove-trs-256.png", &iconHW, &iconHW, &channels, 4);
	if (iconData) {
		GLFWimage icon;
		icon.height = ICONHW;
		icon.width = ICONHW;
		icon.pixels = iconData;
		glfwSetWindowIcon(window, 1, &icon);
	}
	else std::cout << "Load window icon data failed!" << std::endl;

	std::cout << "Setting up BGFX..." << std::endl;
	bgfx::init();
	bgfx::Init config;
	bgfx::PlatformData pdata;
	pdata.nwh = glfwGetWin32Window(window);
	config.type = bgfx::RendererType::OpenGL;
	config.resolution.width = WINDOW_WIDTH;
	config.resolution.height = WINDOW_HEIGHT;
	config.resolution.reset = BGFX_RESET_VSYNC;
	config.platformData = pdata;
	bgfx::init(config);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwMakeContextCurrent(window);
	bgfx::setDebug(BGFX_DEBUG_TEXT);
	std::cout << "Prerequisites all set." << std::endl;
	struct PosColorVertex {
		float x;
		float y;
		float z;
		//uint32_t abgr;
	};
	static PosColorVertex cubeVertices[] = {
		{-1.0f,  1.0f },
		{ 1.0f,  1.0f },
		{-1.0f, -1.0f }
	};
	static const uint16_t cubeTriList[] = {
		0, 2, 1,
	};
	//bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	bgfx::VertexLayout pcvDecl;
	pcvDecl.begin().add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float).end();
	bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
	bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));
	const char *vsc = "vs.bin", *fsc = "fs.bin";
	Shader shader(vsc, fsc);

	unsigned int counter = 0;
	while (!glfwWindowShouldClose(window)) {
		//const bx::Vec3 at = { 0.0f, 0.0f,  0.0f };
		//const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };
		//float view[16];
		//bx::mtxLookAt(view, eye, at);
		//float proj[16];
		//bx::mtxProj(proj, 60.0f, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
		//bgfx::setViewTransform(0, view, proj);
		bgfx::setVertexBuffer(0, vbh);
		bgfx::setIndexBuffer(ibh);
		//bgfx::setViewTransform(0, view, proj);
		//float mtx[16];
		//bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
		//bgfx::setTransform(mtx);
		bgfx::submit(0, shader.program);
		bgfx::touch(0);
		bgfx::frame();
		glfwPollEvents();
		counter++;
		if (counter % 100 == 0) std::cout << "Rendered " << counter << " frames" << std::endl;
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	bgfx::shutdown();
	return 0;
}