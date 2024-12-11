#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <iostream>
#include <bx/string.h>
#include <bx/readerwriter.h>
#include <bx/timer.h>
#include <bx/file.h>
#include <bx/math.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_bgfx.h>

#include "Renderer.hpp"

typedef uint32_t u32;

using std::cout, std::endl;

unsigned int Renderer::instanceCount = 0;
vector<GLFWmonitorfun> Renderer::monitorCBs;

Renderer::Renderer(unsigned int width, unsigned int height, const char* title, const char* iconFilePath) {
	if (instanceCount == 0 && !glfwInit()) {
		cout << "Failed to set up GLFW!" << endl;
		exit(-1);
	}
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) {
		if(instanceCount == 0) glfwTerminate();
		cout << "Failed to create GLFW window!" << endl;
		DebugBreak();
	}
	if (iconFilePath) {
		cout << "Loading window icon from " << iconFilePath << "..." << endl;
		int iconWidth, iconHeight;
		unsigned char* iconData = stbi_load(iconFilePath, &iconWidth, &iconHeight, NULL, 4);
		if (iconData) {
			GLFWimage icon{};
			icon.width = iconWidth;
			icon.height = iconHeight;
			icon.pixels = iconData;
			glfwSetWindowIcon(window, 1, &icon);
			stbi_image_free(iconData);
		}
		else cout << "Load window icon data failed!" << endl;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	bgfx::Init config;
	bgfx::PlatformData pdata;
	pdata.nwh = getNativeHandle(window);
	//makes bgfx autoselect its rendering backend.
	config.type = bgfx::RendererType::Count;
	//Control for switching to OpenGL temporaily for debug.
	//config.type = bgfx::RendererType::OpenGL;
	config.resolution.width = width;
	config.resolution.height = height;
	config.resolution.reset = BGFX_RESET_VSYNC;
	config.platformData = pdata;
	bgfx::init(config);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
	bgfx::setDebug(BGFX_DEBUG_TEXT);
	//bgfx::setState(BGFX_STATE_BLEND_ALPHA | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
	//bgfx::setViewClear(1, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
	cout << "Used rendering backend: " << bgfx::getRendererName(bgfx::getRendererType()) << endl;
	float _scale;
	glfwGetWindowContentScale(window, &_scale, NULL);
	context = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.DisplaySize.x = width;
	io.DisplaySize.y = height;
	io.ConfigViewportsNoAutoMerge = true;
	io.ConfigViewportsNoTaskBarIcon = true;
	io.FontGlobalScale = _scale;
	float scaled_font_size = 14.0f * _scale;
	io.Fonts->AddFontFromFileTTF("assets/fonts/LXGWWenKai-Regular.ttf", scaled_font_size, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	io.Fonts->Build();
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.0f);
	//Code for multiviewport. Not integrated yet.
	//ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	//platform_io.Renderer_CreateWindow = renderer_create_window;
	//platform_io.Renderer_DestroyWindow = renderer_destroy_window;
	//platform_io.Renderer_SetWindowSize = renderer_set_window_size;
	//platform_io.Renderer_RenderWindow = renderer_render_window;
	//platform_io.Renderer_SwapBuffers = nullptr;
	ImGui_ImplGlfw_InitForOther(window, false);
	ImGui_Implbgfx_Init(1);
	glfwSetWindowUserPointer(window, this);
	//Register input callbacks
	glfwSetKeyCallback(window, proxyKeyCB);
	addKeyCB(ImGui_ImplGlfw_KeyCallback);
	glfwSetCharCallback(window, proxyCharCB);
	addCharCB(ImGui_ImplGlfw_CharCallback);
	glfwSetCursorPosCallback(window, proxyCursorPosCB);
	addCursorPosCB(ImGui_ImplGlfw_CursorPosCallback);
	glfwSetCursorEnterCallback(window, proxyCursorEnterCB);
	addCursorEnterCB(ImGui_ImplGlfw_CursorEnterCallback);
	glfwSetMouseButtonCallback(window, proxyMouseButtonCB);
	addMouseButtonCB(ImGui_ImplGlfw_MouseButtonCallback);
	glfwSetScrollCallback(window, proxyScrollCB);
	addScrollCB(ImGui_ImplGlfw_ScrollCallback);
	glfwSetDropCallback(window, proxyDropCB);
	//No dropping callbacks available
	//addDropCB(ImGui_ImplGlfw_DropCallback);
	glfwSetWindowFocusCallback(window, proxyWindowFocusCB);
	addWindowFocusCB(ImGui_ImplGlfw_WindowFocusCallback);
	glfwSetWindowSizeCallback(window, proxyWindowSizeCB);
	//No resize callbacks available
	//addWindowSizeCB(ImGui_ImplGlfw_WindowSizeCallback);
	//Global
	glfwSetMonitorCallback(proxyMonitorCB);
	addMonitorCB(ImGui_ImplGlfw_MonitorCallback);
	instanceCount++;
	windowAlive = true;
}

Renderer::~Renderer() {
	if (windowAlive) windowAlive = false;
	ImGui_Implbgfx_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext(context);
	glfwSetWindowShouldClose(window, 1);
	glfwDestroyWindow(window);
	if (instanceCount == 1) glfwTerminate();
	if (instanceCount == 1) bgfx::shutdown();
	//todo:destroy any other buffers
	instanceCount--;
}

bool Renderer::isAlive() {
	if (glfwWindowShouldClose(window)) windowAlive = false;
	return windowAlive;
}

void Renderer::close() {
	glfwSetWindowShouldClose(window, 1);
	windowAlive = false;
}

//GLFW

GLFWwindow* Renderer::getWindow() { return window; }

HWND Renderer::getNativeHandle(GLFWwindow* window) {
	//Will change someday soon :)
	return glfwGetWin32Window(window);
}

unsigned int Renderer::getWidth() {
	int width;
	glfwGetWindowSize(window, &width, NULL);
	return width;
}

void Renderer::setWidth(int width) { glfwSetWindowSize(window, width, getHeight()); }

unsigned int Renderer::getHeight() {
	int height;
	glfwGetWindowSize(window, NULL, &height);
	return height;
}

void Renderer::setHeight(int height) { glfwSetWindowSize(window, getWidth(), height); }

float Renderer::getWindowScale() {
	float scale;
	glfwGetWindowContentScale(window, &scale, NULL);
	return scale;
}

double Renderer::getAspectRatio() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return (double)width / (double)height;
}

void Renderer::setAspectRatio(int widthRatio, int heightRatio) { glfwSetWindowAspectRatio(window, widthRatio, heightRatio); }

const char* Renderer::getTitle() { return glfwGetWindowTitle(window); }

void Renderer::setTitle(const char* newTitle) { glfwSetWindowTitle(window, newTitle); }

void Renderer::addKeyCB(GLFWkeyfun func) {
	for (u32 i = 0; i < keyCBs.size(); i++) if (keyCBs[i] == func) return;
	keyCBs.push_back(func);
}
void Renderer::removeKeyCB(GLFWkeyfun func) { for (u32 i = 0; i < keyCBs.size(); i++) if (keyCBs[i] == func) keyCBs.erase(keyCBs.begin() + i); }
void Renderer::proxyKeyCB(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Renderer* instance = (Renderer*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->keyCBs.size(); i++) instance->keyCBs[i](window, key, scancode, action, mods);
}

void Renderer::addCharCB(GLFWcharfun func) {
	for (u32 i = 0; i < charCBs.size(); i++) if (charCBs[i] == func) return;
	charCBs.push_back(func);
}
void Renderer::removeCharCB(GLFWcharfun func) { for (u32 i = 0; i < charCBs.size(); i++) if (charCBs[i] == func) charCBs.erase(charCBs.begin() + i); }
void Renderer::proxyCharCB(GLFWwindow* window, unsigned int codepoint) {
	Renderer* instance = (Renderer*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->charCBs.size(); i++) instance->charCBs[i](window, codepoint);
}

void Renderer::addCursorPosCB(GLFWcursorposfun func) {
	for (u32 i = 0; i < cursorposCBs.size(); i++) if (cursorposCBs[i] == func) return;
	cursorposCBs.push_back(func);
}
void Renderer::removeCursorPosCB(GLFWcursorposfun func) { for (u32 i = 0; i < cursorposCBs.size(); i++) if (cursorposCBs[i] == func) cursorposCBs.erase(cursorposCBs.begin() + i); }
void Renderer::proxyCursorPosCB(GLFWwindow* window, double xpos, double ypos) {
	Renderer* instance = (Renderer*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->cursorposCBs.size(); i++) instance->cursorposCBs[i](window, xpos, ypos);
}

void Renderer::addCursorEnterCB(GLFWcursorenterfun func) {
	for (u32 i = 0; i < cursorenterCBs.size(); i++) if (cursorenterCBs[i] == func) return;
	cursorenterCBs.push_back(func);
}
void Renderer::removeCursorEnterCB(GLFWcursorenterfun func) { for (u32 i = 0; i < cursorenterCBs.size(); i++) if (cursorenterCBs[i] == func) cursorenterCBs.erase(cursorenterCBs.begin() + i); }
void Renderer::proxyCursorEnterCB(GLFWwindow* window, int entered) {
	Renderer* instance = (Renderer*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->cursorenterCBs.size(); i++) instance->cursorenterCBs[i](window, entered);
}

void Renderer::addMouseButtonCB(GLFWmousebuttonfun func) {
	for (u32 i = 0; i < mousebuttonCBs.size(); i++) if (mousebuttonCBs[i] == func) return;
	mousebuttonCBs.push_back(func);
}
void Renderer::removeMouseButtonCB(GLFWmousebuttonfun func) { for (u32 i = 0; i < mousebuttonCBs.size(); i++) if (mousebuttonCBs[i] == func) mousebuttonCBs.erase(mousebuttonCBs.begin() + i); }
void Renderer::proxyMouseButtonCB(GLFWwindow* window, int button, int action, int mods) {
	Renderer* instance = (Renderer*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->mousebuttonCBs.size(); i++) instance->mousebuttonCBs[i](window, button, action, mods);
}

void Renderer::addScrollCB(GLFWscrollfun func) {
	for (u32 i = 0; i < scrollCBs.size(); i++) if (scrollCBs[i] == func) return;
	scrollCBs.push_back(func);
}
void Renderer::removeScrollCB(GLFWscrollfun func) { for (u32 i = 0; i < scrollCBs.size(); i++) if (scrollCBs[i] == func) scrollCBs.erase(scrollCBs.begin() + i); }
void Renderer::proxyScrollCB(GLFWwindow* window, double xoffset, double yoffset) {
	Renderer* instance = (Renderer*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->scrollCBs.size(); i++) instance->scrollCBs[i](window, xoffset, yoffset);
}

void Renderer::addDropCB(GLFWdropfun func) {
	for (u32 i = 0; i < dropCBs.size(); i++) if (dropCBs[i] == func) return;
	dropCBs.push_back(func);
}
void Renderer::removeDropCB(GLFWdropfun func) { for (u32 i = 0; i < dropCBs.size(); i++) if (dropCBs[i] == func) dropCBs.erase(dropCBs.begin() + i); }
void Renderer::proxyDropCB(GLFWwindow* window, int count, const char** paths) {
	Renderer* instance = (Renderer*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->dropCBs.size(); i++) instance->dropCBs[i](window, count, paths);
}

void Renderer::addWindowFocusCB(GLFWwindowfocusfun func) {
	for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowFocusCBs[i] == func) return;
	windowFocusCBs.push_back(func);
}
void Renderer::removeWindowFocusCB(GLFWwindowfocusfun func) { for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowFocusCBs[i] == func) windowFocusCBs.erase(windowFocusCBs.begin() + i); }
void Renderer::proxyWindowFocusCB(GLFWwindow* window, int focused) {
	Renderer* instance = (Renderer*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->windowFocusCBs.size(); i++) instance->windowFocusCBs[i](window, focused);
}

void Renderer::addWindowSizeCB(GLFWwindowsizefun func) {
	for (u32 i = 0; i < windowSizeCBs.size(); i++) if (windowSizeCBs[i] == func) return;
	windowSizeCBs.push_back(func);
}
void Renderer::removeWindowSizeCB(GLFWwindowsizefun func) { for (u32 i = 0; i < windowFocusCBs.size(); i++) if (windowSizeCBs[i] == func) windowSizeCBs.erase(windowSizeCBs.begin() + i); }
void Renderer::proxyWindowSizeCB(GLFWwindow* window, int width, int height) {
	Renderer* instance = (Renderer*)glfwGetWindowUserPointer(window);
	for (u32 i = 0; i < instance->windowSizeCBs.size(); i++) instance->windowSizeCBs[i](window, width, height);
}

void Renderer::addMonitorCB(GLFWmonitorfun func) {
	for (u32 i = 0; i < monitorCBs.size(); i++) if (monitorCBs[i] == func) return;
	monitorCBs.push_back(func);
}
void Renderer::removeMonitorCB(GLFWmonitorfun func) { for (u32 i = 0; i < monitorCBs.size(); i++) if (monitorCBs[i] == func) monitorCBs.erase(monitorCBs.begin() + i); }
void Renderer::proxyMonitorCB(GLFWmonitor* monitor, int event) {
	for (u32 i = 0; i < monitorCBs.size(); i++) monitorCBs[i](monitor, event);
}

//ImGui

ImGuiContext* Renderer::getGuiContext() { return context; }

void Renderer::startGuiFrame() {
	bgfx::setViewRect(1, 0, 0, getWidth(), getHeight());
	ImGui_Implbgfx_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Renderer::submitGuiFrame() {
	ImGui::End();
	ImGui::Render();
	ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
	//GLFWwindow* backup_current_context = glfwGetCurrentContext();
	//ImGui::UpdatePlatformWindows();
	//ImGui::RenderPlatformWindowsDefault();
	//glfwMakeContextCurrent(backup_current_context);
}



/* Code for possibly multi viewport.
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
*/