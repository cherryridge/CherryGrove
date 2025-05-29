#pragma once
#include <cstdint>

#include "Logger.hpp"

namespace Fatal {
    typedef int32_t i32;

    //0 is reserved.
    //Integers below 0 are for possible overflows in assigned error code ranges.

    //SDL/Window System/GUI System: 1-1000
    inline constexpr i32 SDL_INITIALIZATION_FALILED                 = 1;
    inline constexpr i32 SDL_CREATE_WINDOW_FAILED                   = 2;

    //bgfx/Renderer: 1001-2000
    inline constexpr i32 BGFX_INITIALIZATION_FALILED                 = 1001;
    inline constexpr i32 BGFX_NO_VALID_RENDER_BACKEND                = 1002;
    inline constexpr i32 BGFX_SHADER_FILE_NOT_FOUND                  = 1003;
    inline constexpr i32 BGFX_OPEN_SHADER_FILE_FAILED                = 1004;
    inline constexpr i32 BGFX_READ_SHADER_FILE_FAILED                = 1005;
    inline constexpr i32 BGFX_GET_SHADER_FAILED                      = 1006;
    //:)
    inline constexpr i32 TEXTUREPOOL_MISSING_MISSING_PNG             = 1007;

    //SoLoud/Sound System: 2001-3000
    inline constexpr i32 SOLOUD_INITIALIZATION_FALILED               = 2001;

    //Pack/Pack runtime fatal error: 3001-4000
    inline constexpr i32 PACK_MALFORMED_CONFIG_JSON                  = 3001;

    //InputHandler: 4001-5000
    

    //Misc: 5001-6000
    inline constexpr i32 MISC_MULTIPLE_INSTANCES                     = 5001;
    inline constexpr i32 MISC_UNSUPPORTED_PLATFORM                   = 5002;

    [[noreturn]] inline void exit(i32 code) noexcept {
        lerr << "[Fatal] Exit code: " << code << endl;
        std::abort();
    }
}