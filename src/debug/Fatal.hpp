#pragma once

#include "Logger.hpp"

namespace Fatal {
    typedef int32_t i32;
    using std::abort, Logger::LOGGER_DYNAMIC_ERR;
    #define DEF inline constexpr i32

    //0 is reserved.
    //Integers below 0 are for possible overflows in assigned error code ranges.

    //SDL/Window System/GUI System: 1-1000
    DEF SDL_INITIALIZATION_FALILED            = 1;
    DEF SDL_CREATE_WINDOW_FAILED              = 2;

    //bgfx/Renderer: 1001-2000
    DEF BGFX_INITIALIZATION_FALILED           = 1001;
    DEF BGFX_NO_VALID_RENDER_BACKEND          = 1002;
    DEF BGFX_SHADER_FILE_NOT_FOUND            = 1003;
    DEF BGFX_OPEN_SHADER_FILE_FAILED          = 1004;
    DEF BGFX_READ_SHADER_FILE_FAILED          = 1005;
    DEF BGFX_GET_SHADER_FAILED                = 1006;
    DEF TEXTUREPOOL_MISSING_MISSING_PNG       = 1007;

    //SoLoud/Sound System: 2001-3000
    DEF SOLOUD_INITIALIZATION_FALILED         = 2001;

    //Pack/Pack runtime fatal error: 3001-4000
    DEF PACK_MALFORMED_CONFIG_JSON            = 3001;
    DEF PACK_SCHEMA_FILE_NOT_FOUND            = 3002;

    //InputHandler: 4001-5000
    

    //Misc: 5001-6000
    
    //File System/Physfs: 6001-7000
    DEF FILESYSTEM_NO_WRITE_PERMISSION        = 6001;
    DEF FILESYSTEM_CANNOT_GET_EXECUTABLE_PATH = 6002;
    DEF FILESYSTEM_CANNOT_WRITE_FILE          = 6003;
    DEF FILESYSTEM_CANNOT_INIT_PHYSFS         = 6004;
    DEF FILESYSTEM_WORKING_DIRECTORY_ILLEGAL  = 6005;
    
    //Boot: 7001-8000
    DEF BOOT_MULTIPLE_INSTANCES               = 7001;
    DEF BOOT_UNSUPPORTED_PLATFORM             = 7002;
    DEF BOOT_INVALID_WORKING_DIR              = 7003;

    //Settings: 8001-9000
    DEF SETTINGS_FAILED_TO_LOAD               = 8001;
    DEF SETTINGS_FAILED_TO_SAVE               = 8002;

    //It shouldn't be here (i.e. there is 100% a bug): 9001-10000

    [[noreturn]] inline void exit(i32 code) noexcept {
        LOGGER_DYNAMIC_ERR("[Fatal] Exit code: ", code);
        abort();
    }
}