#define NOMINMAX
#include <bgfx/bgfx.h>
#include <iostream>
#include <fstream>
#include <bx/math.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>
#include <d3d11.h>

#include "logger/logger.hpp"
#include "graphic/Renderer.hpp"
#include "graphic/texture/TexturePool.hpp"
#include "graphic/shader/ShaderPool.hpp"
#include "js/JSEngine.hpp"
#include "input/keyboard/keyboard.hpp"
#include "input/mouse/mouse.hpp"
#include "input/mouse/scroll.hpp"
#include "input/mouse/cursor.hpp"

using std::cout, std::endl;

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

	cout << "Setting up Renderer..." << endl;
	Renderer renderer(1920, 1080, "CherryGrove", "assets/icons/CherryGrove-trs-64.png");

	JSEngine engine(_argv[0]);
	
	TexturePool texturePool("s_texture");
	unsigned int debugTexture = texturePool.addTexture("assets/textures/debug.png");

	ShaderPool shaderPool;
	unsigned int shader = shaderPool.addShader("test.vert.bin", "test.frag.bin");

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
	bgfx::IndexBufferHandle vertexIndexBuffer1 = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

	const bx::Vec3 at  = {  0.0f,  0.0f,  0.0f };
	const bx::Vec3 eye = {  0.0f,  3.0f,  0.0f };
	float fov = 60.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	
	unsigned int counter = 0;
	bool show = true;
	while (renderer.isAlive()) {
		renderer.startGuiFrame();
		ImGui::ShowDemoWindow(&show);
		ImGui::Begin(" ", &show, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
		ImVec2 size = ImGui::GetWindowSize();
		ImGui::SetWindowPos(ImVec2((renderer.getWidth() - size.x) / 2.0f, (renderer.getHeight() - size.y) / 2.0f));
		ImGui::Text("CherryGrove");
		ImGui::Button("Game");
		ImGui::Button("Settings");
		ImGui::Button("Packs");
		if (ImGui::Button("Exit")) renderer.close();
		renderer.submitGuiFrame();
		bgfx::setViewRect(0, 0, 0, renderer.getWidth(), renderer.getHeight());
		float view[16];
		bx::mtxLookAt(view, eye, at);
		float proj[16];
		bx::mtxProj(proj, fov, renderer.getAspectRatio(), nearPlane, farPlane, bgfx::getCaps()->homogeneousDepth);
		bgfx::setViewTransform(0, view, proj);
		float mtx1[16];
		bx::mtxRotateXY(mtx1, counter * 0.01f, counter * 0.01f);
		bgfx::setTransform(mtx1);
		bgfx::setVertexBuffer(0, vertexBuffer2);
		bgfx::setIndexBuffer(vertexIndexBuffer2);
		shaderPool.useShader(0, shader);
		float mtx2[16];
		if(counter > 100) bx::mtxRotateX(mtx2, -0.5f);
		else bx::mtxRotateX(mtx2, 0.0f);
		bgfx::setTransform(mtx2);
		bgfx::setVertexBuffer(0, vertexBuffer1);
		bgfx::setIndexBuffer(vertexIndexBuffer1);
		texturePool.useTexture(debugTexture);
		shaderPool.useShader(0, shader);
		bgfx::frame();
		counter++;
		if (counter % 100 == 0) cout << "Rendered " << counter << " frames" << endl;
		glfwSwapBuffers(renderer.getWindow());
		glfwPollEvents();
	}
	bgfx::destroy(vertexBuffer1);
	bgfx::destroy(vertexIndexBuffer1);
	bgfx::destroy(vertexBuffer2);
	bgfx::destroy(vertexIndexBuffer2);
	return 0;
}