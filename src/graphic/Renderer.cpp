#include <thread>
#include <atomic>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#if defined(_WIN32)
	#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__linux__)
	#if defined(USE_WAYLAND)
		#define GLFW_EXPOSE_NATIVE_WAYLAND
	#else
		#define GLFW_EXPOSE_NATIVE_X11
	#endif
#elif defined(__APPLE__)
	#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>
#include <stb/stb_image.h>
#define IMGUI_ENABLE_FREETYPE
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_bgfx.h>

#include "../debug/debug.hpp"
#include "../CherryGrove.hpp"
#include "ShaderPool.hpp"
#include "TexturePool.hpp"
#include "../input/InputHandler.hpp"
#include "../gui/Guis.hpp"
#include "../gui/MainWindow.hpp"
#include "../MainGame.hpp"
#include "../components/Components.hpp"
#include "Renderer.hpp"

namespace Renderer {
	typedef int32_t i32;
	typedef uint32_t u32;
	using std::thread, std::atomic, std::unique_lock, MainGame::gameRegistry, MainGame::playerEntity, MainGame::registryMutex;

	atomic<bool> initialized(false);
	atomic<bool> sizeUpdateSignal(true);
	i32 cachedWidth, cachedHeight;
	float cachedAspectRatio;
	static void renderLoop();
	thread rendererThread;
	bgfx::VertexBufferHandle vertexBuffer;
	bgfx::IndexBufferHandle indexBuffer;
	ImGuiContext* context;

	void start() { rendererThread = thread(&renderLoop); }

	void waitShutdown() { rendererThread.join(); }

	static void initBgfx_r() {
		bgfx::Init config;
		bgfx::PlatformData pdata;
		#if defined(_WIN32)
			pdata.ndt = nullptr;
			pdata.nwh = glfwGetWin32Window(MainWindow::window);
			//Temporary IME disabling code!!!
			MainWindow::runOnMainThread([]() {
				ImmAssociateContext(glfwGetWin32Window(MainWindow::window), nullptr);
			});
		#elif defined(__linux__)
			#ifdef USE_WAYLAND
				pdata.ndt = glfwGetWaylandDisplay();
			#else
				pd.ndt = glfwGetX11Display();
				pd.nwh = reinterpret_cast<void*>(glfwGetX11Window(MainWindow::window));
			#endif
		#elif defined(__APPLE__)
			pd.ndt = nullptr;
			pd.nwh = glfwGetCocoaWindow(window);
		#endif
		//Let bgfx auto select rendering backend.
		config.type = bgfx::RendererType::Count;
		//Control for switching backend temporaily for debug.
		//config.type = bgfx::RendererType::Vulkan;
		i32 width, height;
		glfwGetWindowSize(MainWindow::window, &width, &height);
		config.resolution.width = width;
		config.resolution.height = height;
		config.resolution.reset = BGFX_RESET_VSYNC;
		config.platformData = pdata;
		if (!bgfx::init(config)) {
			lerr << "[Renderer] Failed to initialize bgfx!" << endl;
			Fatal::exit(Fatal::BGFX_INITIALIZATION_FALILED);
		}
		lout << "Using rendering backend: " << bgfx::getRendererName(bgfx::getRendererType()) << endl;
		auto caps = bgfx::getCaps();

	//Initialize pools
		ShaderPool::init();
		TexturePool::init("s_texture");

	//Initialize vertex layout
		bgfx::VertexLayout layout;
		layout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, true).add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true).end();
		vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(&blockVerticesTemplate, sizeof(blockVerticesTemplate)), layout);
		indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(&blockIndicesTemplate, sizeof(blockIndicesTemplate)));
	}

	static void initImGui_r() {
		i32 width, height;
		glfwGetWindowSize(MainWindow::window, &width, &height);
		context = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.LogFilename = nullptr;
		io.IniFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.DisplaySize.x = (float)width;
		io.DisplaySize.y = (float)height;
		io.ConfigViewportsNoAutoMerge = true;
		io.ConfigViewportsNoTaskBarIcon = true;
		float scale;
		glfwGetWindowContentScale(MainWindow::window, &scale, nullptr);
		io.FontGlobalScale = scale;
		float scaledFontSize = 24.0f * scale;
		io.Fonts->Flags |= ImFontAtlasFlags_::ImFontAtlasFlags_NoPowerOfTwoHeight;
		io.Fonts->Clear();
		io.FontGlobalScale = 1.0f;
		io.Fonts->AddFontFromFileTTF("assets/fonts/unifont.otf", scaledFontSize, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		io.Fonts->AddFontFromFileTTF("assets/fonts/unifont.otf", 1.4f * scaledFontSize, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		io.Fonts->Build();
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.0f);
		ImGui_ImplGlfw_InitForOther(MainWindow::window, false);
		ImGui_Implbgfx_Init(guiViewId);
		InputHandler::submitImGuiCBs(ImGui_ImplGlfw_KeyCallback, ImGui_ImplGlfw_CharCallback, ImGui_ImplGlfw_CursorPosCallback, ImGui_ImplGlfw_CursorEnterCallback, ImGui_ImplGlfw_MouseButtonCallback, ImGui_ImplGlfw_ScrollCallback, ImGui_ImplGlfw_WindowFocusCallback, ImGui_ImplGlfw_MonitorCallback);
		InputHandler::sendToImGui = true;
	}

	static void shutDownImGui() {
		ImGui_Implbgfx_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(context);
	}

	static void renderLoop() {
		using namespace Components;
		lout << "Renderer" << flush;
		lout << "Hello from renderer thread!" << endl;
		while (CherryGrove::isCGAlive) {
		//Initialize bgfx in the same thread.
			if (!initialized) {
				initBgfx_r();
				initImGui_r();
				initialized = true;
				continue;
			}
		//Process renderer-cycle input events.
			InputHandler::processInputRenderer();
		//Prepare for rendering
			//Refresh window size
			if (sizeUpdateSignal) {
				glfwGetWindowSize(MainWindow::window, &cachedWidth, &cachedHeight);
				cachedAspectRatio = (float)cachedWidth / cachedHeight;
				bgfx::reset(cachedWidth, cachedHeight);
				sizeUpdateSignal = false;
			}
			bgfx::setDebug(BGFX_DEBUG_STATS | BGFX_DEBUG_PROFILER | BGFX_DEBUG_TEXT);
		//Render GUI
			bgfx::setViewClear(guiViewId, BGFX_CLEAR_NONE, 0x00000000);
			bgfx::setViewRect(guiViewId, 0, 0, cachedWidth, cachedHeight);
			ImGui_Implbgfx_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			Guis::render(cachedWidth, cachedHeight);
			ImGui::Render();
			ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
		//Render game content
			bgfx::setViewClear(gameViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x5bd093ff);
			bgfx::setViewRect(gameViewId, 0, 0, cachedWidth, cachedHeight);
			if (MainGame::gameStarted) {
			//Prepare render environment
				float view[16]{}, proj[16]{};
				Rotation::getViewMtx(view, playerEntity);
				Camera::getProjMtx(proj, playerEntity, cachedAspectRatio);
				bgfx::setViewTransform(gameViewId, view, proj);
			//Render opaque parts/blocks?
				//Wait for the lock
				//unique_lock lock(MainGame::registryMutex);
				//Get all renderable blocks
				auto group = gameRegistry.group<const BlockCoordinatesComponent, const BlockRenderComponent>();
				group.each([](entt::entity entity, const BlockCoordinatesComponent& coords, const BlockRenderComponent& renderData) {
					float worldSpaceTranslate[16]{};
					bx::mtxTranslate(worldSpaceTranslate, (float)coords.x, (float)coords.y, (float)coords.z);
					for (const auto& [cubeIndex, cube] : renderData.subcubes) {
						float subcubeTransform[16]{};
						//Subcube translate
						bx::mtxTranslate(subcubeTransform, cube.origin.x, cube.origin.y, cube.origin.z);
						//todo: Subcube rotation
						//bx::mtxRotateXYZ(transform, );
						for (i32 i = 0; i < 6; i++) if(cube.faces[i].shaderId) {
							float transform[16]{};
							bx::mtxMul(transform, worldSpaceTranslate, subcubeTransform);
							bgfx::setTransform(transform);
							bgfx::setVertexBuffer(0, vertexBuffer, i * 4, 4);
							bgfx::setIndexBuffer(indexBuffer);
							TexturePool::useTexture(cube.faces[i].textureId);
							bgfx::submit(gameViewId, ShaderPool::getShader(cube.faces[i].shaderId));
						}
					}
				});
				//lock.unlock();
			//Render translucent blocks

			}
			//We must remind bgfx of view 0 actually exists, or we will be in trouble.
			//We don't need to check the game's status. In the early development, it's really easy to make a mistake and not submitting any draw calls to this view.
			bgfx::touch(gameViewId);
		//Update screen!
			bgfx::frame();
		}
		lout << "Terminating renderer thread!" << endl;
		shutDownImGui();
		TexturePool::shutdown();
		ShaderPool::shutdown();
		bgfx::shutdown();
	}
}