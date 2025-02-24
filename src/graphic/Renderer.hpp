#pragma once
#include <atomic>
#include <array>
#include "renderInfo/vertex.hpp"

namespace Renderer {
	extern std::atomic<bool> initialized;
	
	//Must be called after `MainWindow::initInputHandler`.
	void start();
	void waitShutdown();

	void test();

	//(up, down, north, east, south, west)
	constexpr std::array<Vertex, 24> blockVertexTemplate = {
		//Up
		Vertex{ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
		Vertex{ 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
		Vertex{ 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
		Vertex{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
		//Down
		Vertex{ 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		Vertex{ 1.0f, 0.0f, 1.0f, 1.0f, 0.0f },
		Vertex{ 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		Vertex{ 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		//North (-Z)
		Vertex{ 1.0f, 1.0f, 0.0f, 0.0f, 0.0f },
		Vertex{ 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
		Vertex{ 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		Vertex{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		//East (+X)
		Vertex{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
		Vertex{ 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
		Vertex{ 1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		Vertex{ 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
		//South (+Z)
		Vertex{ 0.0f, 1.0f, 1.0f, 0.0f, 0.0f },
		Vertex{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
		Vertex{ 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		Vertex{ 1.0f, 0.0f, 1.0f, 1.0f, 1.0f },
		//West (-X)
		Vertex{ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
		Vertex{ 0.0f, 1.0f, 1.0f, 1.0f, 0.0f },
		Vertex{ 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		Vertex{ 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
	};

	constexpr std::array<int32_t, 36> blockIndicesTemplate = {
		 0,  2,  1,  1,  2,  3,
		 4,  6,  5,  5,  6,  7,
		 8, 10,  9,  9, 10, 11,
		12, 14, 13, 13, 14, 15,
		16, 18, 17, 17, 18, 19,
		20, 22, 21, 21, 22, 23,
	};
}