#pragma execution_character_set("utf-8")
//#define NOMINMAX
#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <iostream>
#include <memory>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/quaternion.hpp>

#include "debug/Logger.hpp"
#include "gui/Window.hpp"
#include "graphic/TexturePool.hpp"
#include "graphic/ShaderPool.hpp"
#include "pack/PackManager.hpp"
#include "input/keyboard/keyboard.hpp"
#include "input/mouse/mouse.hpp"
#include "input/mouse/scroll.hpp"
#include "input/mouse/cursor.hpp"
#include "graphic/renderInfo/vertex.hpp"
#include "sound/SoLoudWrapper.hpp"

typedef uint16_t u16;

using Logger::lout, std::endl, std::make_unique, std::unique_ptr, std::filesystem::exists, std::filesystem::current_path, std::filesystem::canonical, std::filesystem::path;

glm::vec3 lookingAt = { 0.0f, 0.0f, 0.0f };
glm::vec3 cameraPos = { 0.0f, 3.0f, 0.0f };
float fov = 60.0f, nearPlane = 0.1f, farPlane = 100.0f, moveSpeed = 0.5f, rotateAngle = glm::radians(1.0f);
void keyCallback1(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	glm::vec3 forward = glm::normalize(lookingAt - cameraPos), up = glm::normalize(glm::vec3(forward.x, forward.y, 1.0f)), right = glm::cross(forward, up);
	switch (key) {
		case GLFW_KEY_W:
			cameraPos += forward * moveSpeed;
			lookingAt += forward * moveSpeed;
			break;
		case GLFW_KEY_S:
			cameraPos -= forward * moveSpeed;
			lookingAt -= forward * moveSpeed;
			break;
		case GLFW_KEY_A:
			cameraPos -= right * moveSpeed;
			lookingAt -= right * moveSpeed;
			break;
		case GLFW_KEY_D:
			cameraPos += right * moveSpeed;
			lookingAt += right * moveSpeed;
			break;
		case GLFW_KEY_LEFT_SHIFT:
			cameraPos.z += moveSpeed;
			lookingAt.z += moveSpeed;
			break;
		case GLFW_KEY_SPACE:
			cameraPos.z -= moveSpeed;
			lookingAt.z -= moveSpeed;
			break;
		case GLFW_KEY_Q:
			//lookingAt += glm::angleAxis(rotateAngle, glm::normalize(right)) * glm::normalize(lookingAt - cameraPos);
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rotateAngle, right);
			glm::vec4 wtf = rotation * glm::vec4(forward, 1.0f);
			glm::vec3 wtf2 = glm::vec3(wtf);
			lookingAt = cameraPos + wtf2;
			break;
		case GLFW_KEY_E:
			rotation = glm::rotate(glm::mat4(1.0f), -rotateAngle, right);
			wtf = rotation * glm::vec4(forward, 1.0f);
			wtf2 = glm::vec3(wtf);
			lookingAt = cameraPos + wtf2;
			//lookingAt -= glm::angleAxis(rotateAngle, glm::normalize(right)) * glm::normalize(lookingAt - cameraPos);
			break;
		case GLFW_KEY_Z:
			//lookingAt += glm::angleAxis(rotateAngle, glm::normalize(up)) * glm::normalize(lookingAt - cameraPos);
			break;
		case GLFW_KEY_C:
			//lookingAt -= glm::angleAxis(rotateAngle, glm::normalize(up)) * glm::normalize(lookingAt - cameraPos);
			break;
	}
	lout << "Looking at: (" << lookingAt.x << ", " << lookingAt.y << ", " << lookingAt.z << ")" << endl;
	lout << "Pos: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")" << endl;
}

//Whether the program is running as a console program or a Win32 window program.
#ifdef CG_CONSOLE
int main(int argc, char* argv[]) {
	char** _argv = argv;
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	char** _argv = __argv;
#endif //CG_CONSOLE

	if (!IsDebuggerPresent()) {
		path exePath(_argv[0]);
		path exeDir = canonical(exePath).parent_path();
		current_path(exeDir);
	}
	cout << "Setting up logger..." << endl;

#ifndef CG_CONSOLE
	Logger::setToFile(true);
#endif

#ifdef _DEBUG
	if (IsDebuggerPresent()) {
		//Hacks for auto compiling shaders every time the program starts, while Visual Studio build events does not trigger when nothing in the main code has changed since last build.
		//Sadly only for Visual Studio debugging.
		//lout << "Compiling shaders at runtime for debug..." << endl;
		//current_path("scripts");
		//system("compile_shaders.bat");
		//current_path("..");
	}
#endif //_DEBUG

	lout << "Hello from Logger!" << endl;

	lout << "Working directory: " << current_path() << endl;

	lout << "Setting up main window..." << endl;
	unique_ptr<Window> mainWindow = make_unique<Window>(2560, 1440, "CherryGrove");
	PackManager::init();
	PackManager::refreshPacks();
	
	TexturePool::init("s_texture");
	auto debugpx = TexturePool::addTexture("assets/textures/debug+x.png");
	auto debugnx = TexturePool::addTexture("assets/textures/debug-x.png");
	auto debugpy = TexturePool::addTexture("assets/textures/debug+y.png");
	auto debugny = TexturePool::addTexture("assets/textures/debug-y.png");
	auto debugpz = TexturePool::addTexture("assets/textures/debug+z.png");
	auto debugnz = TexturePool::addTexture("assets/textures/debug-z.png");

	ShaderPool::init();
	auto testShader = ShaderPool::addShader("test.vert.bin", "test.frag.bin");

	SoLoudWrapper::init();
	auto click = SoLoudWrapper::addEvent(SoLoudWrapper::addSound("assets/sounds/click1.ogg"), 2.0f, 1.0f, 0.0f, true);
	//Perform sound engine test if test files are present.
	if (exists("test/a.ogg") && exists("test/b.ogg")) {
		auto soundtest = SoLoudWrapper::addSound("test/a.ogg");
		auto sound2 = SoLoudWrapper::addSound("test/b.ogg");
		auto soundevent1 = SoLoudWrapper::addEvent(soundtest, 1.0f, 1.0f, 1.0f, true);
		DiCoord d{ vec3(0.0f, 0.0f, 0.0f), 0 };
		auto play = SoLoudWrapper::play(soundevent1, d, 0.0f, 1);
		//Sleep(500);
		//auto play2 = SoLoudWrapper::play(soundevent1, d, 0.0f, 1);
		auto soundevent2 = SoLoudWrapper::addEvent(soundtest, 0.3f, 1.0f, 1.0f, true);
		//Sleep(500);
		//auto play3 = SoLoudWrapper::play(soundevent2, d, 0.0f, 1);
		auto soundevent223 = SoLoudWrapper::addEvent(sound2, 3.0f, 1.0f, 1.0f, true);
		//auto play4 = SoLoudWrapper::play(soundevent223, d, 0.0f, 1);
	}

	static Vertex
	vpx[] = {
		{  0.5f, -0.5f,  0.5f,  0.0f,  0.0f }, //G
		{  0.5f,  0.5f,  0.5f,  1.0f,  0.0f }, //E
		{  0.5f, -0.5f, -0.5f,  0.0f,  1.0f }, //B
		{  0.5f,  0.5f, -0.5f,  1.0f,  1.0f }, //D
	},
	vnx[] = {
		{ -0.5f,  0.5f,  0.5f,  0.0f,  1.0f }, //H
		{ -0.5f, -0.5f,  0.5f,  1.0f,  1.0f }, //F
		{ -0.5f,  0.5f, -0.5f,  0.0f,  0.0f }, //C
		{ -0.5f, -0.5f, -0.5f,  1.0f,  0.0f }, //A
	},
	vpy[] = {
		{  0.5f,  0.5f,  0.5f,  0.0f,  0.0f }, //E
		{ -0.5f,  0.5f,  0.5f,  1.0f,  0.0f }, //H
		{  0.5f,  0.5f, -0.5f,  0.0f,  1.0f }, //D
		{ -0.5f,  0.5f, -0.5f,  1.0f,  1.0f }, //C
	},
	vny[] = {
		{ -0.5f, -0.5f,  0.5f,  0.0f,  0.0f }, //F
		{  0.5f, -0.5f,  0.5f,  1.0f,  0.0f }, //G
		{ -0.5f, -0.5f, -0.5f,  0.0f,  1.0f }, //A
		{  0.5f, -0.5f, -0.5f,  1.0f,  1.0f }, //B
	},
	vpz[] = {
		{ -0.5f,  0.5f, -0.5f,  0.0f,  0.0f }, //H
		{ -0.5f, -0.5f, -0.5f,  1.0f,  0.0f }, //F
		{  0.5f,  0.5f, -0.5f,  0.0f,  1.0f }, //E
		{  0.5f, -0.5f, -0.5f,  1.0f,  1.0f }, //G
	},
	vnz[] = {
		{  0.5f,  0.5f,  0.5f,  0.0f,  1.0f }, //D
		{  0.5f, -0.5f,  0.5f,  0.0f,  0.0f }, //B
		{ -0.5f,  0.5f,  0.5f,  1.0f,  1.0f }, //C
		{ -0.5f, -0.5f,  0.5f,  1.0f,  0.0f }, //A
	};
	
	static const u16 indexBufferData[] = {
		0, 1, 2, 2, 1, 3
	};
	bgfx::VertexLayout layout;
	layout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, true).add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true).end();
	bgfx::VertexBufferHandle vbpx = bgfx::createVertexBuffer(bgfx::makeRef(vpx, sizeof(vpx)), layout);
	bgfx::VertexBufferHandle vbnx = bgfx::createVertexBuffer(bgfx::makeRef(vnx, sizeof(vnx)), layout);
	bgfx::VertexBufferHandle vbpy = bgfx::createVertexBuffer(bgfx::makeRef(vpy, sizeof(vpy)), layout);
	bgfx::VertexBufferHandle vbny = bgfx::createVertexBuffer(bgfx::makeRef(vny, sizeof(vny)), layout);
	bgfx::VertexBufferHandle vbpz = bgfx::createVertexBuffer(bgfx::makeRef(vpz, sizeof(vpz)), layout);
	bgfx::VertexBufferHandle vbnz = bgfx::createVertexBuffer(bgfx::makeRef(vnz, sizeof(vnz)), layout);
	bgfx::IndexBufferHandle indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indexBufferData, sizeof(indexBufferData)));
	unsigned int counter = 0;
	bool showGuis = true, demoOpened = false, winPosInitialized = false;

	mainWindow->getInput()->addKeyCB(keyCallback);
	mainWindow->getInput()->addKeyCB(keyCallback1, false);
	mainWindow->getInput()->addMouseButtonCB(mouseCallback);
	mainWindow->getInput()->addScrollCB(scrollCallback);
	mainWindow->getInput()->addCursorPosCB(cursorPosCallback);
	mainWindow->setIcon("assets/icons/CherryGrove-trs-64.png");

	lout << bgfx::getCaps()->limits.maxTextureSize << endl;

	while (mainWindow->isAlive()) {
		mainWindow->startGuiFrame();
		if (showGuis) {
			if(demoOpened) ImGui::ShowDemoWindow(&demoOpened);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 10.0f));
			ImGui::Begin(" ", &showGuis, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
			ImVec2 size = ImGui::GetWindowSize();
			ImGui::SetWindowPos(ImVec2((mainWindow->getWidth() - size.x) / 2.0f, (mainWindow->getHeight() - size.y) / 2.0f));
			ImGuiIO& io = ImGui::GetIO();
			ImGui::PushFont(io.Fonts->Fonts[1]);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 50.0f));
			ImGui::Text("CherryGrove");
			ImGui::PopStyleVar();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 30.0f));
			ImGui::PopFont();
			auto wSize = ImGui::GetWindowSize();
			ImGui::SetCursorPosX((wSize.x - 240.0f) / 2);
			DiCoord d{ vec3(0.0f, 0.0f, 0.0f), 0 };
			if (ImGui::Button(reinterpret_cast<const char*>(u8"存档"), ImVec2(240.0f, 80.0f))) {
				SoLoudWrapper::play(click, d, 0.0f, 1);
				showGuis = false;
			}
			ImGui::SetCursorPosX((wSize.x - 240.0f) / 2);
			if (ImGui::Button(reinterpret_cast<const char*>(u8"内容包"), ImVec2(240.0f, 80.0f))) {
				SoLoudWrapper::play(click, d, 0.0f, 1);
			}
			ImGui::SetCursorPosX((wSize.x - 240.0f) / 2);
			if (ImGui::Button(reinterpret_cast<const char*>(u8"设置"), ImVec2(240.0f, 80.0f))) {
				SoLoudWrapper::play(click, d, 0.0f, 1);
			}
			ImGui::SetCursorPosX((wSize.x - 240.0f) / 2);
			if (ImGui::Button(reinterpret_cast<const char*>(u8"退出"), ImVec2(240.0f, 80.0f))) {
				SoLoudWrapper::play(click, d, 0.0f, 1);
				mainWindow->close();
			}
			ImGui::PopStyleVar();
			ImGui::End();
			ImGui::Begin("  ", &showGuis, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
			if (!winPosInitialized) ImGui::SetWindowPos(ImVec2(10.0f, mainWindow->getHeight() - 60.0f));
			winPosInitialized = true;
			ImGui::Text("©2025 LJM12914. Licensed under GPL-3.0-or-later.");
			ImGui::End();
			ImGui::PopStyleVar(1);
			ImGui::PopStyleColor(2);
		}
		mainWindow->submitGuiFrame();
		mainWindow->startFrame();
		//bgfx::setState(BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
		bgfx::setViewRect(0, 0, 0, mainWindow->getWidth(), mainWindow->getHeight());
		float view[16];
		bx::mtxLookAt(view, bx::Vec3(cameraPos.x, cameraPos.y, cameraPos.z), bx::Vec3(lookingAt.x, lookingAt.y, lookingAt.z));
		float proj[16];
		bx::mtxProj(proj, fov, mainWindow->getAspectRatio(), nearPlane, farPlane, bgfx::getCaps()->homogeneousDepth);
		bgfx::setViewTransform(0, view, proj);

		float mtx2[16];
		bx::mtxRotateXY(mtx2, 0.002f * counter, 0.002f * counter);
		bgfx::setTransform(mtx2);
		//bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
		bgfx::setVertexBuffer(0, vbpx);
		bgfx::setIndexBuffer(indexBuffer);
		TexturePool::useTexture(debugpx);
		mainWindow->addDrawcall(0, testShader);
		//bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
		bgfx::setTransform(mtx2);
		bgfx::setVertexBuffer(0, vbnx);
		bgfx::setIndexBuffer(indexBuffer);
		TexturePool::useTexture(debugnx);
		mainWindow->addDrawcall(0, testShader);
		//bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
		bgfx::setTransform(mtx2);
		bgfx::setVertexBuffer(0, vbpy);
		bgfx::setIndexBuffer(indexBuffer);
		TexturePool::useTexture(debugpy);
		mainWindow->addDrawcall(0, testShader);
		//bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
		bgfx::setTransform(mtx2);
		bgfx::setVertexBuffer(0, vbny);
		bgfx::setIndexBuffer(indexBuffer);
		TexturePool::useTexture(debugny);
		mainWindow->addDrawcall(0, testShader);
		//bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
		bgfx::setTransform(mtx2);
		bgfx::setVertexBuffer(0, vbpz);
		bgfx::setIndexBuffer(indexBuffer);
		TexturePool::useTexture(debugpz);
		mainWindow->addDrawcall(0, testShader);
		//bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
		bgfx::setTransform(mtx2);
		bgfx::setVertexBuffer(0, vbnz);
		bgfx::setIndexBuffer(indexBuffer);
		TexturePool::useTexture(debugnz);
		mainWindow->addDrawcall(0, testShader);
		mainWindow->submitFrame();
		counter++;
		//if (counter % 100 == 0) lout << "Rendered " << counter << " frames" << endl;
		mainWindow->update();
		//SoLoudWrapper::update();
	}
	bgfx::destroy(vbpx);
	bgfx::destroy(vbnx);
	bgfx::destroy(vbpy);
	bgfx::destroy(vbny);
	bgfx::destroy(vbpz);
	bgfx::destroy(vbnz);
	bgfx::destroy(indexBuffer);
	SoLoudWrapper::shutdown();
	ShaderPool::shutdown();
	TexturePool::shutdown();
	PackManager::shutdown();
	//bgfx::destroy(vertexBuffer2);
	//bgfx::destroy(vertexIndexBuffer2);
	Logger::shutdown();
	return 0;
}