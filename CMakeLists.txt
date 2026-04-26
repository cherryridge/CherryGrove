cmake_minimum_required(VERSION "4.0")

project("CherryGrove")

#------------Basic-----------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)
set(CMAKE_SUPPRESS_REGENERATION ON CACHE BOOL "" FORCE)
#----------------------------------

#--------------Tweaks--------------
add_library(ScopedFlags INTERFACE)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    message("Using MSVC / clang-cl")
    target_compile_options(ScopedFlags INTERFACE
        $<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>
        $<$<COMPILE_LANGUAGE:CXX>:/Zc:preprocessor>
        $<$<COMPILE_LANGUAGE:CXX>:/bigobj>
        $<$<COMPILE_LANGUAGE:C,CXX>:/utf-8>
    )
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # I got a linker error about "__setusermatherr is replaced" when I link using lld and I have no idea why. This is a black magic workaround that seems to work: use MSVC.
    set(CMAKE_LINKER_TYPE "MSVC" CACHE STRING "" FORCE)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message("Using clang / clang-cl")
    add_compile_options(
        # I hate clang
        # —Yes I know about `-Wno-error` but I don't want to disable all warnings as errors.
        # If you run into some warnings that does not exist in this list, please add it here via PR.
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unused-command-line-argument>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unqualified-std-cast-call>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unused-variable>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-macro-redefined>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-missing-field-initializers>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-invalid-offsetof>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unused-function>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unused-private-field>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-unused-but-set-variable>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-missing-braces>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-macro-redefined>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-deprecated-declarations>
        $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-microsoft-void-pseudo-dtor>
    )
endif()

if(WIN32)
    # Override everything to Windows 10 SDK
    add_compile_options(
        $<$<COMPILE_LANGUAGE:C,CXX>:/U_WIN32_WINNT>
        $<$<COMPILE_LANGUAGE:C,CXX>:/UWINVER>
        $<$<COMPILE_LANGUAGE:C,CXX>:/UNTDDI_VERSION>
    )
    add_compile_definitions(
        $<$<COMPILE_LANGUAGE:C,CXX>:WIN32_LEAN_AND_MEAN>
        $<$<COMPILE_LANGUAGE:C,CXX>:NOMINMAX>
        $<$<COMPILE_LANGUAGE:C,CXX>:_WIN32_WINNT=0x0A00>
        $<$<COMPILE_LANGUAGE:C,CXX>:WINVER=0x0A00>
        $<$<COMPILE_LANGUAGE:C,CXX>:NTDDI_VERSION=0x0A000000>
    )
endif()
#----------------------------------

#------------Clangd----------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
#----------------------------------

#------------gperf-base------------
find_program(GPERF_EXECUTABLE NAMES gperf)
if(NOT GPERF_EXECUTABLE)
    message(FATAL_ERROR "gperf not found! Please install gperf and ensure it is in your PATH.")
endif()

function(gperf_generate IN OUT)
    if(NOT IN)
        message(FATAL_ERROR "gperf_generate: missing IN")
    endif()
    if(NOT OUT)
        message(FATAL_ERROR "gperf_generate: missing OUT")
    endif()
    set(IN_ ${CMAKE_SOURCE_DIR}/${IN})
    set(OUT_ ${CMAKE_BINARY_DIR}/${OUT})
    add_custom_command(
        OUTPUT ${OUT_}
        COMMAND ${GPERF_EXECUTABLE} --language=C++ --readonly-tables --output-file=${OUT_} ${IN_}
        DEPENDS ${IN_}
        COMMENT "Generating ${OUT_} from ${IN_} with gperf"
        VERBATIM
    )
    set_source_files_properties(${OUT_} PROPERTIES GENERATED TRUE)
    add_custom_target(gperf_${OUT} DEPENDS ${OUT_})
    add_dependencies(${PROJECT_NAME} gperf_${OUT})
endfunction()
#----------------------------------

#------------Dep:Prebuilt----------
set(CG_PREBUILT_ARCH_SOURCE "")
if(CMAKE_CXX_COMPILER_TARGET)
    set(CG_PREBUILT_ARCH_SOURCE "${CMAKE_CXX_COMPILER_TARGET}")
elseif(CMAKE_C_COMPILER_TARGET)
    set(CG_PREBUILT_ARCH_SOURCE "${CMAKE_C_COMPILER_TARGET}")
else()
    set(CG_PREBUILT_ARCH_SOURCE "${CMAKE_SYSTEM_PROCESSOR}")
endif()
string(TOLOWER "${CG_PREBUILT_ARCH_SOURCE}" CG_PREBUILT_ARCH_SOURCE)
if(CG_PREBUILT_ARCH_SOURCE MATCHES "^(amd64|x64|x86_64)(-|$)")
    set(CG_PREBUILT_ARCH_TAG "x64")
elseif(CG_PREBUILT_ARCH_SOURCE MATCHES "^(arm64|aarch64)(-|$)")
    set(CG_PREBUILT_ARCH_TAG "arm64")
else()
    message(FATAL_ERROR "Unsupported target architecture for prebuilt libraries: ${CG_PREBUILT_ARCH_SOURCE}")
endif()
message(STATUS "Detected target architecture for prebuilt libraries: ${CG_PREBUILT_ARCH_TAG}")

set(BGFX_BGFX_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bgfx$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")
set(BGFX_BX_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bx$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")
set(BGFX_BIMG_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bimg$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")

set(SOLOUD_LIB "${CMAKE_SOURCE_DIR}/libs/soloud/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>soloud_static_${CG_PREBUILT_ARCH_TAG}$<$<CONFIG:Debug>:_d>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")

set(V8_LIB "${CMAKE_SOURCE_DIR}/libs/v8/$<IF:$<CONFIG:Debug>,debug,release>/$<IF:$<PLATFORM_ID:Windows>,v8_monolith.lib,libv8_monolith.a>")

set(WASMTIME_LIB "${CMAKE_SOURCE_DIR}/libs/wasmtime/$<IF:$<CONFIG:Debug>,debug,release>/$<IF:$<PLATFORM_ID:Windows>,wasmtime.lib,libwasmtime.a>")

if(WIN32)
    set(fetch_prebuilt_libs_script "${CMAKE_SOURCE_DIR}/libs/fetch_prebuilt_libs.ps1")
    find_program(POWERSHELL_EXECUTABLE
        NAMES pwsh pwsh.exe powershell powershell.exe
        REQUIRED
    )
else()
    set(fetch_prebuilt_libs_script "${CMAKE_SOURCE_DIR}/libs/fetch_prebuilt_libs.sh")
endif()

add_custom_target(fetch_prebuilt_libs
    COMMAND "${POWERSHELL_EXECUTABLE}" -NoProfile -ExecutionPolicy Bypass -File "${fetch_prebuilt_libs_script}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/libs"
    COMMENT "Running fetch_prebuilt_libs script..."
    VERBATIM
)
#----------------------------------

#------------Dep:3rdparty----------
set(PHYSFS_ARCHIVE_ZIP         ON  CACHE BOOL "")
set(PHYSFS_ARCHIVE_7Z          ON  CACHE BOOL "")
set(PHYSFS_ARCHIVE_GRP         OFF CACHE BOOL "")
set(PHYSFS_ARCHIVE_WAD         OFF CACHE BOOL "")
set(PHYSFS_ARCHIVE_CSM         OFF CACHE BOOL "")
set(PHYSFS_ARCHIVE_HOG         OFF CACHE BOOL "")
set(PHYSFS_ARCHIVE_MVL         OFF CACHE BOOL "")
set(PHYSFS_ARCHIVE_QPAK        OFF CACHE BOOL "")
set(PHYSFS_ARCHIVE_SL          OFF CACHE BOOL "")
set(PHYSFS_ARCHIVE_ISO9660     OFF CACHE BOOL "")
set(PHYSFS_ARCHIVE_VDF         OFF CACHE BOOL "")
set(PHYSFS_ARCHIVE_LECARCHIVES OFF CACHE BOOL "")
set(PHYSFS_BUILD_STATIC        ON  CACHE BOOL "")
set(PHYSFS_BUILD_SHARED        OFF CACHE BOOL "")
set(PHYSFS_BUILD_TEST          OFF CACHE BOOL "")
set(PHYSFS_DISABLE_INSTALL     ON  CACHE BOOL "")
set(PHYSFS_BUILD_DOCS          OFF CACHE BOOL "")
if(UNIX)
    set(PHYSFS_TARGETNAME_DIST "physfs-dist-please-do-not-collide-with-anything-ok" CACHE STRING "")
endif()
add_subdirectory("libs/physfs")

set(LUAU_BUILD_CLI    OFF CACHE BOOL "")
set(LUAU_BUILD_TESTS  OFF CACHE BOOL "")
set(LUAU_BUILD_WEB    OFF CACHE BOOL "")
set(LUAU_STATIC_CRT   ON  CACHE BOOL "")
set(LUAU_BUILD_SHARED OFF CACHE BOOL "")
add_subdirectory("libs/luau" EXCLUDE_FROM_ALL)

#-----Enjoy the legacy of RocksDB! 100+ Lines just to make it happy and not destroy everything.-----

set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(LZ4_BUILD_CLI     OFF CACHE BOOL "")
add_subdirectory("libs/lz4/build/cmake")
set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "")
set(BUILD_TESTING       OFF CACHE BOOL "")
set(ZSTD_BUILD_STATIC   ON  CACHE BOOL "")
set(ZSTD_BUILD_SHARED   OFF CACHE BOOL "")
if(MSVC OR CMAKE_CXX_SIMULATE_ID MATCHES "MSVC")
    set(ZSTD_USE_STATIC_RUNTIME ON CACHE BOOL "")
endif()
add_subdirectory("libs/zstd/build/cmake")
if(WIN32)
    set(CG_LZ4_LIBRARY "${CMAKE_BINARY_DIR}/libs/lz4/build/cmake/lz4.lib")
    set(CG_ZSTD_LIBRARY "${CMAKE_BINARY_DIR}/libs/zstd/build/cmake/lib/zstd_static.lib")
else()
    set(CG_LZ4_LIBRARY "${CMAKE_BINARY_DIR}/libs/lz4/build/cmake/liblz4.a")
    set(CG_ZSTD_LIBRARY "${CMAKE_BINARY_DIR}/libs/zstd/build/cmake/lib/libzstd.a")
endif()
set(lz4_ROOT_DIR     "${CMAKE_SOURCE_DIR}/libs/lz4"     CACHE PATH "" FORCE)
set(lz4_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/libs/lz4/lib" CACHE PATH "" FORCE)
set(lz4_LIBRARIES    "${CG_LZ4_LIBRARY}"                CACHE FILEPATH "" FORCE)
set(zstd_ROOT_DIR    "${CMAKE_SOURCE_DIR}/libs/zstd"    CACHE PATH "" FORCE)
set(ZSTD_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/zstd/lib" CACHE PATH "" FORCE)
set(ZSTD_INCLUDE_DIRS "${ZSTD_INCLUDE_DIR}"             CACHE PATH "" FORCE)
set(ZSTD_LIBRARY     "${CG_ZSTD_LIBRARY}"               CACHE FILEPATH "" FORCE)
set(ZSTD_LIBRARIES   "${CG_ZSTD_LIBRARY}"               CACHE FILEPATH "" FORCE)
if(WIN32)
    set(ZSTD_LIBRARY_DEBUG "${CG_ZSTD_LIBRARY}" CACHE FILEPATH "" FORCE)
    set(ZSTD_LIBRARY_RELEASE "${CG_ZSTD_LIBRARY}" CACHE FILEPATH "" FORCE)
endif()
set(ENV{LZ4_HOME} "${CMAKE_SOURCE_DIR}/libs/lz4")
set(ENV{LZ4_INCLUDE} "${CMAKE_SOURCE_DIR}/libs/lz4/lib")
set(ENV{LZ4_LIB_DEBUG} "${CG_LZ4_LIBRARY}")
set(ENV{LZ4_LIB_RELEASE} "${CG_LZ4_LIBRARY}")
set(ENV{ZSTD_HOME} "${CMAKE_SOURCE_DIR}/libs/zstd")
set(ENV{ZSTD_INCLUDE} "${CMAKE_SOURCE_DIR}/libs/zstd/lib")
set(ENV{ZSTD_LIB_DEBUG} "${CG_ZSTD_LIBRARY}")
set(ENV{ZSTD_LIB_RELEASE} "${CG_ZSTD_LIBRARY}")
set(WITH_LZ4                    ON  CACHE BOOL "")
set(WITH_ZSTD                   ON  CACHE BOOL "")
set(WITH_WINDOWS_UTF8_FILENAMES ON  CACHE BOOL "")
set(ROCKSDB_BUILD_SHARED        OFF CACHE BOOL "")
set(WITH_MD_LIBRARY             OFF CACHE BOOL "")
set(WITH_GFLAGS                 OFF CACHE BOOL "")
set(WITH_TESTS                  OFF CACHE BOOL "")
set(WITH_BENCHMARK_TOOLS        OFF CACHE BOOL "")
set(WITH_TOOLS                  OFF CACHE BOOL "")
set(WITH_CORE_TOOLS             OFF CACHE BOOL "")
set(WITH_TRACE_TOOLS            OFF CACHE BOOL "")
set(WITH_ALL_TESTS              OFF CACHE BOOL "")
# OK. They just DONT CARE about whether the main project needs lz4 or other compression library.
# THEY JUST LINK THEIRS PRIVATELY. ALWAYS.
# I GIVE UP.
if(WIN32)
    set(ROCKSDB_SKIP_THIRDPARTY OFF CACHE BOOL "")
endif()
# Why the fuck does this exist on non-Linux builds?
if(UNIX AND NOT APPLE)
    set(WITH_LIBURING ON  CACHE BOOL "")
else()
    set(WITH_LIBURING OFF CACHE BOOL "")
endif()
# Why the fuck does this option even exist?
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(WITH_RUNTIME_DEBUG ON  CACHE BOOL "")
else()
    set(WITH_RUNTIME_DEBUG OFF CACHE BOOL "")
endif()
add_subdirectory("libs/rocksdb")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    # util/auto_tune_compressor.cc(98)
    target_compile_options(rocksdb PRIVATE /wd4702)
    # rocksdb/include/rocksdb/advanced_options.h(632)
    target_compile_options(rocksdb PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:/utf-8>)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    get_property(ROCKSDB_TARGETS DIRECTORY "${CMAKE_SOURCE_DIR}/libs/rocksdb" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(ROCKSDB_TARGET IN LISTS ROCKSDB_TARGETS)
        get_target_property(ROCKSDB_TARGET_TYPE "${ROCKSDB_TARGET}" TYPE)
        if(NOT ROCKSDB_TARGET_TYPE STREQUAL "UTILITY" AND NOT ROCKSDB_TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
            # RocksDB already hardcodes /MT and /MTd in its own MSVC flags.
            # Prevent CMake from adding an extra clang-style -MT/-MTd that
            # swallows the next option under clang-cl.
            set_property(TARGET "${ROCKSDB_TARGET}" PROPERTY MSVC_RUNTIME_LIBRARY "")
            target_compile_options("${ROCKSDB_TARGET}" PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:-Wno-error=unused-command-line-argument>)
        endif()
    endforeach()
endif()

#-----Did you enjoy it?-----

set(FT_DISABLE_ZLIB OFF CACHE BOOL "")
add_subdirectory("libs/freetype")

set(SDL_SHARED        OFF CACHE BOOL "")
set(SDL_STATIC        ON  CACHE BOOL "")
set(SDL_EXAMPLES      OFF CACHE BOOL "")
set(SDL_TEST_LIBRARY  OFF CACHE BOOL "")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
add_subdirectory("libs/sdl")

set(SDL_DISABLE_INSTALL  ON  CACHE BOOL "")
set(SDLIMAGE_BACKEND_STB ON  CACHE BOOL "")
set(SDLIMAGE_STRICT      ON  CACHE BOOL "")
set(SDLIMAGE_VENDORED    ON  CACHE BOOL "")
set(SDLIMAGE_DEPS_SHARED OFF CACHE BOOL "")
set(SDLIMAGE_ANI        OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_AVIF       OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_BMP        ON  CACHE BOOL "" FORCE)
set(SDLIMAGE_GIF        OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_JPG        ON  CACHE BOOL "" FORCE)
set(SDLIMAGE_JXL        OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_LBM        OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_PCX        OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_PNG        ON  CACHE BOOL "" FORCE)
set(SDLIMAGE_PNG_SHARED OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_PNM        OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_QOI        OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_SVG        OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TGA        ON  CACHE BOOL "" FORCE)
set(SDLIMAGE_TIF        OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_WEBP       OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XCF        OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XPM        OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_XV         OFF CACHE BOOL "" FORCE)
add_subdirectory("libs/sdl_image")
#----------------------------------

#------------Dep:Internal----------
set(CGNBT_HAS_BOOST  ON CACHE BOOL "")
set(CGNBT_HAS_PHYSFS ON CACHE BOOL "")
set(CGNBT_HAS_ZSTD   ON CACHE BOOL "")
set(CGNBT_BOOST_DIRECTORY  "${CMAKE_SOURCE_DIR}/libs/boost"  CACHE STRING "")
set(CGNBT_PHYSFS_DIRECTORY "${CMAKE_SOURCE_DIR}/libs/physfs" CACHE STRING "")
set(CGNBT_ZSTD_DIRECTORY   "${CMAKE_SOURCE_DIR}/libs/zstd"   CACHE STRING "")
set(CGNBT_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory("libs/cgnbt")
#----------------------------------

#------------Files-----------------
file(GLOB_RECURSE CG_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE IMGUI_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/libs/imgui/*.cpp")
#----------------------------------

#------------CherryGrove-----------
if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${CG_SOURCES} ${IMGUI_SOURCES})
else()
    add_executable(${PROJECT_NAME} ${CG_SOURCES} ${IMGUI_SOURCES})
endif()
#----------------------------------

#-------------gperf----------------
#----------------------------------

#------------Build:Macro-----------
target_compile_definitions(${PROJECT_NAME} PRIVATE
    # Common usage
    $<IF:$<CONFIG:Debug>,DEBUG,NDEBUG>
    # Required by bx
    BX_CONFIG_DEBUG=$<IF:$<CONFIG:Debug>,1,0>
    # Actual debug flag for CherryGrove
    $<IF:$<CONFIG:Debug>,CG_DEBUG,CG_RELEASE>
)
#----------------------------------

#------------Inclusions------------
target_include_directories(${PROJECT_NAME} PRIVATE
    "libs/imgui"

    "libs/boost/assert/include"
    "libs/boost/config/include"
    "libs/boost/container_hash/include"
    "libs/boost/core/include"
    "libs/boost/describe/include"
    "libs/boost/detail/include"
    "libs/boost/exception/include"
    "libs/boost/mp11/include"
    "libs/boost/pfr/include"
    "libs/boost/predef/include"
    "libs/boost/static_assert/include"
    "libs/boost/throw_exception/include"
    "libs/boost/type_traits/include"
    "libs/boost/unordered/include"
    "libs/boost/utility/include"
    "libs/boost/uuid/include"

    "libs/cli11/include"
    "libs/entt/single_include"
    "libs/function2/include"
    "libs/glaze/include"
    "libs/glm"

    "libs/physfs/src"
    "libs/luau/Compiler/include"
    "libs/luau/VM/include"
    "libs/zstd/lib"
    "libs/rocksdb/include"
    "libs/freetype/include"
    "libs/sdl/include"
    "libs/sdl_image/include"

    "libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/include"
    "libs/soloud/$<IF:$<CONFIG:Debug>,debug,release>/include"
    "libs/v8/$<IF:$<CONFIG:Debug>,debug,release>/include"
    "libs/wasmtime/$<IF:$<CONFIG:Debug>,debug,release>/include"

    "libs/cgnbt/include"
    "${CMAKE_BINARY_DIR}/shaders"
)
#----------------------------------

#---------------Link---------------
target_link_libraries(${PROJECT_NAME} PRIVATE
    ScopedFlags

    physfs-static
    Luau.Compiler
    Luau.VM
    Luau.CodeGen
    libzstd_static
    rocksdb
    freetype
    SDL3-static
    SDL3_image-static

    ${BGFX_BGFX_LIB}
    ${BGFX_BX_LIB}
    ${BGFX_BIMG_LIB}
    ${SOLOUD_LIB}
    ${V8_LIB}
    ${WASMTIME_LIB}
)
#----------------------------------

#-----Tweaks for Prebuilt Libs-----
if(WIN32)
    # Wasmtime on static lib build
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        WASM_API_EXTERN=
        WASI_API_EXTERN=
    )
endif()
#----------------------------------

#------------Link:System-----------
if(WIN32)
    # Luau
    target_link_libraries(${PROJECT_NAME} PRIVATE Kernel32)
    # SoLoud
    target_link_libraries(${PROJECT_NAME} PRIVATE xaudio2)
    # V8
    target_link_libraries(${PROJECT_NAME} PRIVATE dbghelp)
    # Wasmtime
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 advapi32 userenv ntdll shell32 ole32 oleaut32 bcrypt)
    # Packing
    target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/packing/windows")
    target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/packing/windows/CherryGrove.rc")
elseif(UNIX AND NOT APPLE)
    find_package(ALSA REQUIRED)
    find_package(PkgConfig REQUIRED)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${ALSA_LIBRARIES}
        Threads::Threads
        dl
        atomic
    )
elseif(APPLE)
    find_library(AUDIO_TOOLBOX_LIBRARY AudioToolbox)
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(FOUNDATION_LIBRARY Foundation)
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COREAUDIO_LIBRARY}
        ${AUDIO_TOOLBOX_LIBRARY}
        ${METAL_LIBRARY}
        ${FOUNDATION_LIBRARY}
        ${QUARTZCORE_LIBRARY}
    )
endif()
#----------------------------------

#-------Copy Debug Env Files-------
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/packs")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/packs" "${CMAKE_BINARY_DIR}/packs")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/saves")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/saves" "${CMAKE_BINARY_DIR}/saves")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/tests")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/tests" "${CMAKE_BINARY_DIR}/tests")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/settings.json")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/settings.json" "${CMAKE_BINARY_DIR}/settings.json")
endif()
#----------------------------------

#----------Compile Shader----------
file(GLOB VERTEX_SHADERS CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/shader/*.vert")
file(GLOB FRAGMENT_SHADERS CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/shader/*.frag")
set(SHADERC_EXECUTABLE "$<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/libs/bgfx/debug/bin/shaderc,${CMAKE_SOURCE_DIR}/libs/bgfx/release/bin/shaderc>")

function(make_shader_compilation_commands result)
    set(commands)
    foreach(SHADER_PATH IN LISTS VERTEX_SHADERS)
        get_filename_component(SHADER_NAME_EXT "${SHADER_PATH}" NAME)
        get_filename_component(SHADER_NAME "${SHADER_PATH}" NAME_WE)
        list(APPEND commands
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_dx11.h" --platform windows --bin2c "${SHADER_NAME}_vert_dx11" --type vertex -p s_5_0 -O 3
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_dx12.h" --platform windows --bin2c "${SHADER_NAME}_vert_dx12" --type vertex -p s_6_9 -O 3
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_opengl.h" --bin2c "${SHADER_NAME}_vert_opengl" --type vertex -p 440
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_vulkan.h" --bin2c "${SHADER_NAME}_vert_vulkan" --type vertex -p spirv16-13
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_metal.h" --bin2c "${SHADER_NAME}_vert_metal" --type vertex -p metal31-14 --platform osx
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_opengles.h" --bin2c "${SHADER_NAME}_vert_opengles" --type vertex -p 320_es
        )
    endforeach()
    foreach(SHADER_PATH IN LISTS FRAGMENT_SHADERS)
        get_filename_component(SHADER_NAME_EXT "${SHADER_PATH}" NAME)
        get_filename_component(SHADER_NAME "${SHADER_PATH}" NAME_WE)
        list(APPEND commands
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_dx11.h" --platform windows --bin2c "${SHADER_NAME}_frag_dx11" --type fragment -p s_5_0 -O 3
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_dx12.h" --platform windows --bin2c "${SHADER_NAME}_frag_dx12" --type fragment -p s_6_9 -O 3
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_opengl.h" --bin2c "${SHADER_NAME}_frag_opengl" --type fragment -p 440
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_vulkan.h" --bin2c "${SHADER_NAME}_frag_vulkan" --type fragment -p spirv16-13
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_metal.h" --bin2c "${SHADER_NAME}_frag_metal" --type fragment -p metal31-14 --platform osx
            COMMAND ${SHADERC_EXECUTABLE} --verbose -f "${SHADER_PATH}" -i "${CMAKE_SOURCE_DIR}/libs/bgfx/source/src" -o "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME_EXT}_opengles.h" --bin2c "${SHADER_NAME}_frag_opengles" --type fragment -p 320_es
        )
    endforeach()
    set(${result} "${commands}" PARENT_SCOPE)
endfunction()

make_shader_compilation_commands(SHADER_COMPILATION_COMMANDS)

add_custom_target(compile_shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/shaders"
    ${SHADER_COMPILATION_COMMANDS}
    VERBATIM
    COMMAND_EXPAND_LISTS
)
add_dependencies(${PROJECT_NAME} compile_shaders)
#----------------------------------