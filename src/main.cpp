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
#include <bx/math.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <d3d11.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_bgfx.h"

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

void renderer_create_window(ImGuiViewport* viewport) {

	auto data = viewport->PlatformUserData;
	bgfx::FrameBufferHandle handle = bgfx::createFrameBuffer(
		data,
		uint16_t(viewport->Size.x),
		uint16_t(viewport->Size.y));

	viewport->RendererUserData = (void*)(uintptr_t)handle.idx;
	GLFWwindow* new_window = glfwCreateWindow(
		(int)viewport->Size.x,
		(int)viewport->Size.y,
		"ImGui Viewport",
		nullptr,
		nullptr
	);

	viewport->PlatformHandle = (void*)new_window;
	viewport->PlatformHandleRaw = new_window;
	viewport->RendererUserData = (void*)handle.idx;

	// Ensure the new window has the correct position
	glfwSetWindowPos(new_window, (int)viewport->Pos.x, (int)viewport->Pos.y);

	// Associate user pointer for callback use
	glfwSetWindowUserPointer(new_window, viewport);
}

void renderer_destroy_window(ImGuiViewport* viewport) {
	auto data = viewport->PlatformUserData;
	bgfx::FrameBufferHandle handle = {(uint16_t)viewport->RendererUserData };
	bgfx::destroy(handle);
	bgfx::frame();
	bgfx::frame();
	if (GLFWwindow* window = (GLFWwindow*)viewport->PlatformHandle) {
		glfwDestroyWindow(window);
		viewport->PlatformHandle = nullptr;
	}
}

static void renderer_render_window(ImGuiViewport* viewport, void* render_arg) {
	int display_w, display_h;
	glfwGetWindowSize((GLFWwindow*)viewport->PlatformHandle, &display_w, &display_h);
	bgfx::FrameBufferHandle handle = { (uint16_t)(uintptr_t)viewport->RendererUserData };
	bgfx::setViewFrameBuffer(1, handle);
	bgfx::setViewRect(1, 0, 0, uint16_t(display_w), uint16_t(display_h));
	bgfx::setViewClear(1, BGFX_CLEAR_COLOR, 0xff00ffff, 1.0f, 0);
	// Set render states.
	bgfx::setState(BGFX_STATE_DEFAULT);
	ImGui_Implbgfx_RenderDrawLists(viewport->DrawData);
	bgfx::touch(1);
}

//Whether the program is running as a console program or a Win32 window program.
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
	//Hacks for auto compiling shaders every time the program starts, while Visual Studio build events does not trigger when nothing in the main code has changed since last build.
	//Sadly only for Visual Studio debugging.
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
	float xScale, yScale;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwGetWindowContentScale(window, &xScale, &yScale);
	cout << xScale << " " << yScale << endl;
	cout << "Loading window icon..." << endl;
	int garbage;
	unsigned char* iconData = stbi_load("assets/icons/CherryGrove-trs-256.png", &garbage, &garbage, &garbage, 4);
	if (iconData) {
		GLFWimage icon;
		icon.height = ICONHW;
		icon.width = ICONHW;
		icon.pixels = iconData;
		glfwSetWindowIcon(window, 1, &icon);
		stbi_image_free(iconData);
	}
	else cout << "Load window icon data failed!" << endl;
	int actHeight, actWidth;
	glfwGetFramebufferSize(window, &actWidth, &actHeight);
	cout << actHeight << " " << actWidth << endl;
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
	//bgfx::setState(BGFX_STATE_BLEND_ALPHA | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
	//bgfx::setViewClear(1, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
	bgfx::setDebug(BGFX_DEBUG_TEXT);
	cout << "Used rendering backend: " << bgfx::getRendererName(bgfx::getRendererType()) << endl;
	cout << "---------------------------" << endl;

	cout << "Setting up ImGui.." << endl;
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.DisplaySize.x = WINDOW_WIDTH;
	io.DisplaySize.y = WINDOW_HEIGHT;
	io.ConfigViewportsNoAutoMerge = true;
	io.ConfigViewportsNoTaskBarIcon = true;
	io.FontGlobalScale = xScale;
	float scaled_font_size = 14.0f * xScale;
	io.Fonts->AddFontFromFileTTF("assets/fonts/LXGWWenKai-Regular.ttf", scaled_font_size, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	io.Fonts->Build();
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.0f);
	//ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	//platform_io.Renderer_CreateWindow = renderer_create_window;
	//platform_io.Renderer_DestroyWindow = renderer_destroy_window;
	//platform_io.Renderer_SetWindowSize = renderer_set_window_size;
	//platform_io.Renderer_RenderWindow = renderer_render_window;
	//platform_io.Renderer_SwapBuffers = nullptr;
	ImGui_ImplGlfw_InitForOther(window, true);
	ImGui_Implbgfx_Init(1);

	float aspectRatio = float(WINDOW_WIDTH) / float(WINDOW_HEIGHT);

	int height, width, bitsPerPixel;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* textureData = stbi_load("assets/textures/debug.png", &height, &width, &bitsPerPixel, STBI_rgb_alpha);
	bgfx::TextureHandle textureHandle = bgfx::createTexture2D(uint16_t(width), uint16_t(height), false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_NONE | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT, bgfx::makeRef(textureData, height * width * 4));
	bgfx::setState(BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));

	struct PosColorVertex {
		float x;
		float y;
		float z;
		float u;
		float v;
	};
	struct PosVertex {
		float x;
		float y;
		float z;
	};
	static PosVertex bbb[] = {
		{  0.0f,  0.0f, -2.0f },
		{ -0.5f,  0.0f, -2.0f },
		{  0.0f, -0.5f, -2.0f },
	};
	static const uint16_t bbbList[] = {
		0, 1, 2, 0, 2, 1
	};
	bgfx::VertexLayout layout2;
	layout2.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, true).end();
	bgfx::DynamicVertexBufferHandle vertexBuffer2 = bgfx::createDynamicVertexBuffer(bgfx::makeRef(bbb, sizeof(bbb)), layout2);
	bgfx::IndexBufferHandle vertexIndexBuffer2 = bgfx::createIndexBuffer(bgfx::makeRef(bbbList, sizeof(bbbList)));
	static PosColorVertex cubeVertices[] = {
		//
		{ -0.5f, -0.5f, -0.5f,  0.0f,  0.0f },
		{  0.5f, -0.5f, -0.5f,  1.0f,  0.0f },
		{ -0.5f,  0.5f, -0.5f,  0.0f,  1.0f },
		{  0.5f,  0.5f, -0.5f,  1.0f,  1.0f },
		//
		{ -0.5f, -0.5f,  0.5f,  0.0f,  0.0f },
		{  0.5f, -0.5f,  0.5f,  1.0f,  0.0f },
		{ -0.5f, -0.5f, -0.5f,  0.0f,  1.0f },
		{  0.5f, -0.5f, -0.5f,  1.0f,  1.0f },
		//
		{ -0.5f, -0.5f, -0.5f,  0.0f,  0.0f },
		{  0.5f, -0.5f, -0.5f,  1.0f,  0.0f },
		{ -0.5f,  0.5f, -0.5f,  0.0f,  1.0f },
		{  0.5f,  0.5f, -0.5f,  1.0f,  1.0f },
		//
		{ -0.5f, -0.5f, -0.5f,  0.0f,  0.0f },
		{  0.5f, -0.5f, -0.5f,  1.0f,  0.0f },
		{ -0.5f,  0.5f, -0.5f,  0.0f,  1.0f },
		{  0.5f,  0.5f, -0.5f,  1.0f,  1.0f },
		//
		{  0.5f,  0.5f,  0.5f,  0.0f,  0.0f },
		{ -0.5f, -0.5f,  0.5f,  1.0f,  1.0f },
		{  0.5f, -0.5f,  0.5f,  1.0f,  0.0f },
		{ -0.5f,  0.5f,  0.5f,  0.0f,  1.0f },
		//
		{  0.5f,  0.5f,  0.5f,  0.0f,  0.0f },
		{ -0.5f, -0.5f,  0.5f,  1.0f,  1.0f },
		{  0.5f, -0.5f,  0.5f,  1.0f,  0.0f },
		{ -0.5f,  0.5f,  0.5f,  0.0f,  1.0f }
	};
	static const uint16_t cubeTriList[] = {
		0, 1, 2, 2, 1, 3,
	    4, 5, 6, 6, 5, 7
	};
	bgfx::VertexLayout layout1;
	layout1.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, true).add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true).end();
	bgfx::DynamicVertexBufferHandle vertexBuffer1 = bgfx::createDynamicVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), layout1);
	bgfx::UniformHandle sTexture = bgfx::createUniform("s_texture", bgfx::UniformType::Sampler);
	bgfx::IndexBufferHandle vertexIndexBuffer1 = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

	const bx::Vec3 at = {  0.0f,  0.0f,  0.0f };
	const bx::Vec3 eye = {  0.0f,  3.0f,  0.0f };
	float fov = 60.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	
	const char* vsc = "test.vert.bin", * fsc = "test.frag.bin";
	Shader shader(vsc, fsc);
	double time = glfwGetTime();
	unsigned int counter = 0;
	bool show = true;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		bgfx::setViewRect(1, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		ImGui_Implbgfx_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow(&show);
		ImGui::Begin(" ", &show, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
		ImVec2 size = ImGui::GetWindowSize();
		ImGui::SetWindowPos(ImVec2((WINDOW_WIDTH - size.x) / 2.0f, (WINDOW_HEIGHT - size.y) / 2.0f));
		ImGui::Text("CherryGrove");
		ImGui::Button("Game");
		ImGui::Button("Settings");
		ImGui::Button("Packs");
		if (ImGui::Button("Exit")) glfwSetWindowShouldClose(window, 1);
		ImGui::End();
		ImGui::Render();
		ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
		//GLFWwindow* backup_current_context = glfwGetCurrentContext();
		//ImGui::UpdatePlatformWindows();
		//ImGui::RenderPlatformWindowsDefault();
		//glfwMakeContextCurrent(backup_current_context);
		bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		float view[16];
		bx::mtxLookAt(view, eye, at);
		float proj[16];
		bx::mtxProj(proj, fov, aspectRatio, nearPlane, farPlane, bgfx::getCaps()->homogeneousDepth);
		bgfx::setViewTransform(0, view, proj);
		float mtx1[16];
		bx::mtxRotateXY(mtx1, counter * 0.01f, counter * 0.01f);
		bgfx::setTransform(mtx1);
		bgfx::setVertexBuffer(0, vertexBuffer2);
		bgfx::setIndexBuffer(vertexIndexBuffer2);
		bgfx::submit(0, shader.program);
		float mtx2[16];
		if(counter > 100) bx::mtxRotateX(mtx2, -0.5f);
		else bx::mtxRotateX(mtx2, 0.0f);
		bgfx::setTransform(mtx2);
		bgfx::setVertexBuffer(0, vertexBuffer1);
		bgfx::setIndexBuffer(vertexIndexBuffer1);
		bgfx::setTexture(0, sTexture, textureHandle);
		bgfx::submit(0, shader.program);
		bgfx::frame();
		counter++;
		if (counter % 100 == 0) cout << "Rendered " << counter << " frames" << endl;
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	ImGui_Implbgfx_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	stbi_image_free(textureData);
	bgfx::destroy(vertexBuffer1);
	bgfx::destroy(vertexIndexBuffer1);
	bgfx::destroy(textureHandle);
	bgfx::destroy(sTexture);
	bgfx::shutdown();
	return 0;
}