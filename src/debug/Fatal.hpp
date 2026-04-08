#pragma once
#include <cstdlib>

#include "Logger.hpp"

namespace Fatal {
    typedef int32_t i32;
    #define DEF inline constexpr i32

    //0 is reserved.
    //Integers below 0 are for possible overflows in assigned error code ranges.

    //SDL/Window System/GUI System: 1-1000
    DEF SDL_INITIALIZATION_FAILED             = 1;
    DEF SDL_CREATE_WINDOW_FAILED              = 2;

    //bgfx/Renderer: 1001-2000
    DEF BGFX_INITIALIZATION_FAILED            = 1001;
    DEF BGFX_NO_VALID_RENDER_BACKEND          = 1002;
    DEF BGFX_CREATE_SHADER_FAILED             = 1003;
    DEF BGFX_GET_SHADER_FAILED                = 1004;
    DEF TEXTUREPOOL_MISSING_MISSING_PNG       = 1005;
    DEF TEXTUREPOOL_INITIALIZATION_FAILED     = 1006;
    DEF TEXTUREPOOL_ATLAS_CREATION_FAILED     = 1007;

    //SoLoud/Sound System: 2001-3000
    DEF SOLOUD_INITIALIZATION_FAILED          = 2001;

    //Pack/Pack runtime fatal error: 3001-4000

    //InputHandler: 4001-5000
    

    //Misc: 5001-6000
    
    //File System/Physfs: 6001-7000
    DEF FILESYSTEM_NO_WRITE_PERMISSION        = 6001;
    DEF FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH = 6002;
    DEF FILESYSTEM_CANNOT_INIT_PHYSFS         = 6003;
    
    //Boot: 7001-8000
    DEF BOOT_MULTIPLE_INSTANCES               = 7001;
    DEF BOOT_INVALID_WORKING_DIR              = 7002;

    //Settings: 8001-9000
    DEF SETTINGS_FAILED_TO_LOAD               = 8001;
    DEF SETTINGS_FAILED_TO_SAVE               = 8002;

    //It shouldn't be here (i.e. there is 100% a bug): 9001-10000
    DEF TEXTUREPOOL_TEXTURE_NOT_IN_ATLAS     = 9001;

    [[noreturn]] inline void exit(i32 code) noexcept {
        Logger::LOGGER_DYNAMIC_ERR("[Fatal] Exit code: ", code);
        std::exit(code);
    }

    #undef DEF
}