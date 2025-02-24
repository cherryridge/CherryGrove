#include <thread>
#include <atomic>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <glfw/glfw3.h>
#if _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <glfw/glfw3native.h>
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
#include "../gameplay/MainGame.hpp"
#include "../components/Components.hpp"
#include "Renderer.hpp"

namespace Renderer {
	typedef int32_t i32;
	typedef uint32_t u32;

	using std::thread, std::atomic, std::unique_lock, MainGame::gameRegistry, MainGame::playerEntity, MainGame::registryMutex;

	atomic<bool> bgfxInited(false);
	static void renderLoop();
	thread rendererThread;
	
	ImGuiContext* context;
	ShaderPool::ShaderID baseShader;

	void start() { rendererThread = thread(&renderLoop); }

	void waitShutdown() { rendererThread.join(); }

	static void initBgfxForRendererThread() {
		bgfx::Init config;
		bgfx::PlatformData pdata;
		#if _WIN32
		auto handle = glfwGetWin32Window(MainWindow::window);
		//Temporary IME disabling code!!!
		ImmAssociateContext(handle, nullptr);
		#endif
		pdata.nwh = handle;
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
		bgfx::renderFrame();
		if (!bgfx::init(config)) {
			lerr << "[Renderer] Failed to initialize bgfx!" << endl;
			Fatal::exit(Fatal::BGFX_INITIALIZATION_FALILED);
		}
		bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x5BD093FF, 1.0f, 0);
		bgfx::setDebug(BGFX_DEBUG_TEXT);
		lout << "Using rendering backend: " << bgfx::getRendererName(bgfx::getRendererType()) << endl;
		auto caps = bgfx::getCaps();
		//todo
		lout << "Capabilities: " << endl;

	//Initialize pools
		ShaderPool::init();
		TexturePool::init("s_texture");
		baseShader = ShaderPool::addShader("base.vert.bin", "base.frag.bin");

		bgfxInited = true;
	}

	static void initImGui() {
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
		io.Fonts->AddFontFromFileTTF("assets/fonts/unifont.otf", 2.4f * scaledFontSize, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
		io.Fonts->Build();
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.0f);
		ImGui_ImplGlfw_InitForOther(MainWindow::window, false);
		ImGui_Implbgfx_Init(1);
		InputHandler::addKeyCB(ImGui_ImplGlfw_KeyCallback, false);
		InputHandler::addCharCB(ImGui_ImplGlfw_CharCallback, false);
		InputHandler::addCursorPosCB(ImGui_ImplGlfw_CursorPosCallback, false);
		InputHandler::addCursorEnterCB(ImGui_ImplGlfw_CursorEnterCallback, false);
		InputHandler::addMouseButtonCB(ImGui_ImplGlfw_MouseButtonCallback, false);
		InputHandler::addScrollCB(ImGui_ImplGlfw_ScrollCallback, false);
		//No dropping callbacks available
		//addDropCB(ImGui_ImplGlfw_DropCallback, false);
		InputHandler::addWindowFocusCB(ImGui_ImplGlfw_WindowFocusCallback, false);
		//No resize callbacks available
		//addWindowSizeCB(ImGui_ImplGlfw_WindowSizeCallback, false);
		InputHandler::addMonitorCB(ImGui_ImplGlfw_MonitorCallback);
	}

	void test() {
		TexturePool::TextureID
		debugpx = TexturePool::addTexture("assets/textures/debug+x.png"),
		debugnx = TexturePool::addTexture("assets/textures/debug-x.png"),
		debugpy = TexturePool::addTexture("assets/textures/debug+y.png"),
		debugny = TexturePool::addTexture("assets/textures/debug-y.png"),
		debugpz = TexturePool::addTexture("assets/textures/debug+z.png"),
		debugnz = TexturePool::addTexture("assets/textures/debug-z.png");
	}

	static void renderLoop() {
		using namespace Components;
		lout << "Renderer" << flush;
		lout << "Hello from renderer thread!" << endl;
		while (CherryGrove::isCGAlive) {
		//Initialize bgfx in the same thread.
			if (!bgfxInited) {
				initBgfxForRendererThread();
				initImGui();
				continue;
			}
		//Blocks this thread if MainGame is updating the world.
			//unique_lock lock(registryMutex);
		//Prepare for rendering
			u32 width = MainWindow::getWidth(), height = MainWindow::getHeight();
			//A temporary very slow approach for user resizing
			bgfx::reset(width, height);
		//Render GUI
			ImGui::SetCurrentContext(context);
			//View 1 is for GUI, view 0 is for other graphics.
			bgfx::setViewClear(1, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x5bd093ff);
			bgfx::setViewRect(1, 0, 0, width, height);
			ImGui_Implbgfx_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			Guis::render();
			ImGui::Render();
			ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
			//One internal window named `Debug##Default` will always be rendered by ImGui.
			//We don't want to look for the name because this piece of code is performance intensive,
			//instead we only look at the count and adjust the threshold between debug/release if needed.
			InputHandler::hasGUI = context->WindowsActiveCount > 1;
		//If game is started, render the game
			if (MainGame::gameStarted) {
				//Render opaque parts/blocks?
					//Prepare render environment
				bgfx::setViewRect(0, 0, 0, width, height);
				bgfx::setViewTransform(0, Rotation::getViewMtx(gameRegistry, playerEntity), Camera::getProjMtx(gameRegistry, playerEntity, (float)width / height));

				//Prepare for block render template (make them global?)
				bgfx::VertexLayout layout;
				layout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, true).add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true).end();
				bgfx::VertexBufferHandle vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(&blockVertexTemplate, sizeof(blockVertexTemplate)), layout);
				bgfx::IndexBufferHandle indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(&blockIndicesTemplate, sizeof(blockIndicesTemplate)));

				//Get all renderable blocks
				auto group = gameRegistry.group<const BlockCoordinatesComponent, const BlockRenderComponent>();
				group.each([&vertexBuffer, &indexBuffer](entt::entity entity, const BlockCoordinatesComponent& coords, const BlockRenderComponent& renderData) {
					float transform[16]{};
					//Block transform
					bx::mtxTranslate(transform, (float)coords.x, (float)coords.y, (float)coords.z);
					for (const auto& [cubeIndex, cube] : renderData.subcubes) {
						//Subcube translate
						bx::mtxTranslate(transform, cube.origin.x, cube.origin.y, cube.origin.z);
						//todo: Subcube rotation
						//bx::mtxRotateXYZ(transform, );
						for (i32 i = 0; i < 6; i++) {
							//Cubeface translate
							//bx::mtxTranslate(transform, );
							bgfx::setTransform(transform);
							bgfx::setVertexBuffer(0, vertexBuffer, i * 4, 4);
							bgfx::setIndexBuffer(indexBuffer, i * 6, 6);
							TexturePool::useTexture(cube.faces[i].textureId);
							bgfx::submit(0, ShaderPool::getShader(cube.faces[i].shaderId));
						}
					}
				});
				//Render translucent parts

			}
		//Else we must remind bgfx of view 0 actually exists, or we will be in trouble
			else bgfx::touch(0);
		//Update screen!
			bgfx::frame();
		}
		lout << "Terminating renderer thread!" << endl;
		bgfx::renderFrame();
		ImGui_Implbgfx_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext(context);
		TexturePool::shutdown();
		ShaderPool::shutdown();
		bgfx::shutdown();
	}
}