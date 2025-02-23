#pragma once
#include <cstdint>
#include <cstdlib>

#include "Logger.hpp"

namespace Fatal {
	typedef int32_t i32;

	void exit(i32 code);

	//0 is reserved.
	//Integers below 0 are for possible overflows in assigned error code ranges.

	//glfw/Window System/GUI System: 1-1000
	constexpr i32 GLFW_INITIALIZATION_FALILED                 = 1;
	constexpr i32 GLFW_CREATE_WINDOW_FAILED                   = 2;

	//bgfx/Renderer: 1001-2000
	constexpr i32 BGFX_INITIALIZATION_FALILED                 = 1001;
	constexpr i32 BGFX_NO_VALID_RENDER_BACKEND                = 1002;
	constexpr i32 BGFX_SHADER_FILE_NOT_FOUND                  = 1003;
	constexpr i32 BGFX_OPEN_SHADER_FILE_FAILED                = 1004;
	constexpr i32 BGFX_READ_SHADER_FILE_FAILED                = 1005;
	constexpr i32 BGFX_GET_SHADER_FAILED                      = 1006;

	//SoLoud/Sound: 2001-3000
	constexpr i32 SOLOUD_INITIALIZATION_FALILED               = 2001;

	//PackManager/Pack runtime fatal error: 3001-4000
	constexpr i32 PACK_MALFORMED_CONFIG_JSON                  = 3001;
}