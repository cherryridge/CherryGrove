#pragma execution_character_set("utf-8")
#define NOMINMAX
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
#include "graphic/Window.hpp"
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

using std::cout, std::endl, std::make_unique, std::unique_ptr, std::filesystem::exists;

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
	cout << "Looking at: (" << lookingAt.x << ", " << lookingAt.y << ", " << lookingAt.z << ")" << endl;
	cout << "Pos: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")" << endl;
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
	//cout << "Compiling shaders at runtime for debug..." << endl;
	//SetCurrentDirectoryA("scripts");
	//system("compile_shaders.bat");
	//string path = _argv[0];
	//string solutionRootDir = path.substr(0, path.find("\\build\\x64\\Debug"));
	//SetCurrentDirectoryA(solutionRootDir.c_str());
#endif
	cout << "Program running at " << _argv[0] << endl;

	cout << "Setting up main window..." << endl;
	unique_ptr<Window> mainWindow = make_unique<Window>(2560, 1440, "CherryGrove");
	PackManager::init(_argv[0]);
	
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
	//Do sound engine test if test files are present.
	if (exists("test/a.wav") && exists("test/b.wav")) {
		auto soundtest = SoLoudWrapper::addSound("test/a.wav");
		auto sound2 = SoLoudWrapper::addSound("test/b.wav");
		auto soundevent1 = SoLoudWrapper::addEvent(soundtest, 1.0f, 1.0f, 1.0f, true);
		DiCoord d{ vec3(0.0f, 0.0f, 0.0f), 0 };
		auto play = SoLoudWrapper::play(soundevent1, d, 0.0f, 1);
		Sleep(500);
		auto play2 = SoLoudWrapper::play(soundevent1, d, 0.0f, 1);
		auto soundevent2 = SoLoudWrapper::addEvent(soundtest, 0.3f, 1.0f, 1.0f, true);
		Sleep(500);
		auto play3 = SoLoudWrapper::play(soundevent2, d, 0.0f, 1);
		auto soundevent223 = SoLoudWrapper::addEvent(sound2, 3.0f, 1.0f, 1.0f, true);
		auto play4 = SoLoudWrapper::play(soundevent223, d, 0.0f, 1);
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
	bgfx::DynamicVertexBufferHandle vbpx = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vpx, sizeof(vpx)), layout);
	bgfx::DynamicVertexBufferHandle vbnx = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vnx, sizeof(vnx)), layout);
	bgfx::DynamicVertexBufferHandle vbpy = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vpy, sizeof(vpy)), layout);
	bgfx::DynamicVertexBufferHandle vbny = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vny, sizeof(vny)), layout);
	bgfx::DynamicVertexBufferHandle vbpz = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vpz, sizeof(vpz)), layout);
	bgfx::DynamicVertexBufferHandle vbnz = bgfx::createDynamicVertexBuffer(bgfx::makeRef(vnz, sizeof(vnz)), layout);
	bgfx::IndexBufferHandle indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indexBufferData, sizeof(indexBufferData)));
	unsigned int counter = 0;
	bool showGuis = true, demoOpened = false, winPosInitialized = false;

	mainWindow->getInput()->addKeyCB(keyCallback);
	mainWindow->getInput()->addKeyCB(keyCallback1);
	mainWindow->getInput()->addMouseButtonCB(mouseCallback);
	mainWindow->getInput()->addScrollCB(scrollCallback);
	mainWindow->getInput()->addCursorPosCB(cursorPosCallback);
	mainWindow->setIcon("assets/icons/CherryGrove-trs-64.png");

	while (mainWindow->isAlive()) {
		if (showGuis) {
			mainWindow->startGuiFrame();
			if(demoOpened) ImGui::ShowDemoWindow(&demoOpened);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::Begin(" ", &showGuis, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
			ImVec2 size = ImGui::GetWindowSize();
			ImGui::SetWindowPos(ImVec2((mainWindow->getWidth() - size.x) / 2.0f, (mainWindow->getHeight() - size.y) / 2.0f));
			//ImGuiIO& io = ImGui::GetIO();
			//ImGui::PushFont(io.Fonts->Fonts[1]);
			ImGui::Text("CherryGrove");
			//ImGui::PopFont();
			//ImGui::PushFont(io.Fonts->Fonts[0]);
			if (ImGui::Button(reinterpret_cast<const char*>(u8"存档"))) showGuis = false;
			ImGui::Button(reinterpret_cast<const char*>(u8"内容包"));
			ImGui::Button(reinterpret_cast<const char*>(u8"设置"));
			if (ImGui::Button(reinterpret_cast<const char*>(u8"退出"))) mainWindow->close();
			//ImGui::PopFont();
			ImGui::End();
			ImGui::Begin("  ", &showGuis, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
			if (!winPosInitialized) ImGui::SetWindowPos(ImVec2(10.0f, mainWindow->getHeight() - 60.0f));
			winPosInitialized = true;
			ImGui::Text("©2024 LJM12914. Licensed under GPL-3.0-or-later.");
			ImGui::End();
			ImGui::PopStyleColor(2);
			mainWindow->submitGuiFrame();
		}
		mainWindow->startFrame();
		//bgfx::setState(BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
		bgfx::setViewRect(0, 0, 0, mainWindow->getWidth(), mainWindow->getHeight());
		float view[16];
		bx::mtxLookAt(view, bx::Vec3(cameraPos.x, cameraPos.y, cameraPos.z), bx::Vec3(lookingAt.x, lookingAt.y, lookingAt.z));
		float proj[16];
		bx::mtxProj(proj, fov, mainWindow->getAspectRatio(), nearPlane, farPlane, bgfx::getCaps()->homogeneousDepth);
		bgfx::setViewTransform(0, view, proj);
		//float mtx1[16];
		//bx::mtxRotateXY(mtx1, counter * 0.01f, counter * 0.01f);
		//bgfx::setTransform(mtx1);
		//bgfx::setVertexBuffer(0, vertexBuffer2);
		//bgfx::setIndexBuffer(vertexIndexBuffer2);
		//shaderPool.useShader(0, shader);
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
		//if (counter % 100 == 0) cout << "Rendered " << counter << " frames" << endl;
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
	return 0;
}