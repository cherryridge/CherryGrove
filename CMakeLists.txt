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
        $<$<COMPILE_LANGUAGE:CXX>:/utf-8>
    )
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
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
    )
endif()

if(WIN32)
    #Override everything to Windows 10 SDK
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

#------------Files-----------------
file(GLOB_RECURSE CG_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE IMGUI_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/libs/imgui/*.cpp")
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
    set(IN_ ${CMAKE_CURRENT_SOURCE_DIR}/${IN})
    set(OUT_ ${CMAKE_CURRENT_BINARY_DIR}/${OUT})
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
set(BGFX_BGFX_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bgfx$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")
set(BGFX_BX_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bx$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")
set(BGFX_BIMG_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bimg$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")

set(SOLOUD_LIB "${CMAKE_SOURCE_DIR}/libs/soloud/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>soloud_static_x64$<$<CONFIG:Debug>:_d>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")

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
add_subdirectory("libs/luau")

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
set(WITH_ALL_TESTS              OFF CACHE BOOL "")
set(WITH_BENCHMARK_TOOLS        OFF CACHE BOOL "")
#OK. They just DONT CARE about whether the main project needs lz4 or other compression library.
#THEY JUST LINK THEIRS PRIVATELY. ALWAYS.
#I GIVE UP.
if(WIN32)
    set(ROCKSDB_SKIP_THIRDPARTY OFF CACHE BOOL "")
endif()
set(ENV{LZ4_HOME} ${CMAKE_SOURCE_DIR}/libs/lz4)
set(ENV{LZ4_INCLUDE} $ENV{LZ4_HOME}/lib)
set(ENV{LZ4_LIB_DEBUG} ${CMAKE_BINARY_DIR}/libs/lz4/build/cmake/lz4.lib)
set(ENV{LZ4_LIB_RELEASE} ${CMAKE_BINARY_DIR}/libs/lz4/build/cmake/lz4.lib)
set(ENV{ZSTD_HOME} ${CMAKE_SOURCE_DIR}/libs/zstd)
set(ENV{ZSTD_INCLUDE} $ENV{ZSTD_HOME}/lib)
set(ENV{ZSTD_LIB_DEBUG} ${CMAKE_BINARY_DIR}/libs/zstd/build/cmake/lib/zstd_static.lib)
set(ENV{ZSTD_LIB_RELEASE} ${CMAKE_BINARY_DIR}/libs/zstd/build/cmake/lib/zstd_static.lib)
#Why the fuck does this exist on non-Linux builds?
if(UNIX AND NOT APPLE)
    set(WITH_LIBURING ON  CACHE BOOL "")
else()
    set(WITH_LIBURING OFF CACHE BOOL "")
endif()
#Why the fuck does this option even exist?
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(WITH_RUNTIME_DEBUG ON  CACHE BOOL "")
else()
    set(WITH_RUNTIME_DEBUG OFF CACHE BOOL "")
endif()
add_subdirectory("libs/rocksdb")

set(FT_DISABLE_ZLIB OFF CACHE BOOL "")
add_subdirectory("libs/freetype")

set(SDL_SHARED        OFF CACHE BOOL "")
set(SDL_STATIC        ON  CACHE BOOL "")
set(SDL_EXAMPLES      OFF CACHE BOOL "")
set(SDL_TEST_LIBRARY  OFF CACHE BOOL "")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
add_subdirectory("libs/sdl")

set(SDL_DISABLE_INSTALL  ON CACHE BOOL "")
set(SDLIMAGE_BACKEND_STB ON CACHE BOOL "")
set(SDLIMAGE_STRICT      ON CACHE BOOL "")
set(SDLIMAGE_VENDORED    ON CACHE BOOL "")
set(SDLIMAGE_AVIF ON  CACHE BOOL "")
set(SDLIMAGE_BMP  ON  CACHE BOOL "")
set(SDLIMAGE_GIF  OFF CACHE BOOL "")
set(SDLIMAGE_JPG  ON  CACHE BOOL "")
set(SDLIMAGE_JXL  OFF CACHE BOOL "")
set(SDLIMAGE_LBM  OFF CACHE BOOL "")
set(SDLIMAGE_PCX  OFF CACHE BOOL "")
set(SDLIMAGE_PNG  ON  CACHE BOOL "")
set(SDLIMAGE_PNM  OFF CACHE BOOL "")
set(SDLIMAGE_QOI  OFF CACHE BOOL "")
set(SDLIMAGE_SVG  OFF CACHE BOOL "")
set(SDLIMAGE_TGA  ON  CACHE BOOL "")
set(SDLIMAGE_TIF  ON  CACHE BOOL "")
set(SDLIMAGE_WEBP ON  CACHE BOOL "")
set(SDLIMAGE_XCF  OFF CACHE BOOL "")
set(SDLIMAGE_XPM  OFF CACHE BOOL "")
set(SDLIMAGE_XV   OFF CACHE BOOL "")
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

#------------CherryGrove-----------
add_executable(${PROJECT_NAME} ${CG_SOURCES} ${IMGUI_SOURCES})
#----------------------------------

#-------------gperf----------------
#----------------------------------

#------------Build:Macro-----------
target_compile_definitions(${PROJECT_NAME} PRIVATE
    #Common usage
    $<IF:$<CONFIG:Debug>,DEBUG,NDEBUG>
    #Required by bx
    BX_CONFIG_DEBUG=$<IF:$<CONFIG:Debug>,1,0>
    #Actual debug flag for CherryGrove
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
)
#----------------------------------

#---------------Link---------------
target_link_libraries(${PROJECT_NAME} PRIVATE
    ScopedFlags

    physfs-static
    Luau.Compiler
    Luau.VM
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

    CGNBT
)
#----------------------------------

#--------------Tweaks--------------
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    #util/auto_tune_compressor.cc(98): error C2220: the following warning is treated as an error
    target_compile_options(rocksdb PRIVATE /wd4702)
endif()

if(WIN32)
    #Wasmtime on static lib build
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        WASM_API_EXTERN=
        WASI_API_EXTERN=
    )
endif()
#----------------------------------

#------------Link:System-----------
if(WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
    #Luau
    target_link_libraries(${PROJECT_NAME} PRIVATE Kernel32)
    #SoLoud
    target_link_libraries(${PROJECT_NAME} PRIVATE xaudio2 xapobase)
    #V8
    target_link_libraries(${PROJECT_NAME} PRIVATE dbghelp)
    #Wasmtime
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 advapi32 userenv ntdll shell32 ole32 bcrypt)
    #Packing
    target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/packing/windows")
    target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/packing/windows/CherryGrove.rc")
elseif(UNIX AND NOT APPLE)
    find_package(ALSA REQUIRED)
    find_package(PkgConfig REQUIRED)
    find_package(Threads REQUIRED)
    pkg_check_modules(JACK REQUIRED jack)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${ALSA_LIBRARIES}
        ${JACK_LIBRARIES}
        Threads::Threads
        dl
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

#------------Debug:CopyFile--------
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/packs")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/packs" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/packs")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/saves")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/saves" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/saves")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/test")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/test" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/test")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/settings.json")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/settings.json" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/settings.json")
endif()
#----------------------------------

#------------Debug:Shader----------
file(GLOB VERT_SHADER_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/shader/*.vert")
foreach(SHADER_PATH IN LISTS VERT_SHADER_SOURCES)
    get_filename_component(SHADER_NAME "${SHADER_PATH}" NAME)
    if(WIN32)
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/dx11"
            COMMAND $<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/libs/bgfx/debug/shaderc,${CMAKE_SOURCE_DIR}/libs/bgfx/release/shaderc> -f "${SHADER_PATH}" -o "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/dx11/${SHADER_NAME}.bin" --type v -p s_5_0 --platform windows -O 3
            COMMENT "$<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/libs/bgfx/debug/shaderc,${CMAKE_SOURCE_DIR}/libs/bgfx/release/shaderc> -f ${SHADER_PATH} -o $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/dx11/${SHADER_NAME}.bin --type v -p s_5_0 --platform windows -O 3"
            COMMAND_EXPAND_LISTS
            VERBATIM
        )
    endif()
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/glsl"
        COMMAND $<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/libs/bgfx/debug/shaderc,${CMAKE_SOURCE_DIR}/libs/bgfx/release/shaderc> -f "${SHADER_PATH}" -o "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/glsl/${SHADER_NAME}.bin" --type v -p 440 -O 3
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/spirv"
        COMMAND $<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/libs/bgfx/debug/shaderc,${CMAKE_SOURCE_DIR}/libs/bgfx/release/shaderc> -f "${SHADER_PATH}" -o "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/spirv/${SHADER_NAME}.bin" --type v -p spirv16-13 --platform linux -O 3
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/metal"
        COMMAND $<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/libs/bgfx/debug/shaderc,${CMAKE_SOURCE_DIR}/libs/bgfx/release/shaderc> -f "${SHADER_PATH}" -o "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/metal/${SHADER_NAME}.bin" --type v -p metal31-14 --platform osx -O 3
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
endforeach()

file(GLOB FRAG_SHADER_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/shader/*.frag")
foreach(SHADER_PATH IN LISTS FRAG_SHADER_SOURCES)
    get_filename_component(SHADER_NAME "${SHADER_PATH}" NAME)
    if(WIN32)
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/dx11"
            COMMAND $<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/libs/bgfx/debug/shaderc,${CMAKE_SOURCE_DIR}/libs/bgfx/release/shaderc> -f "${SHADER_PATH}" -o "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/dx11/${SHADER_NAME}.bin" --type f -p s_5_0 --platform windows -O 3
            COMMAND_EXPAND_LISTS
            VERBATIM
        )
    endif()
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/glsl"
        COMMAND $<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/libs/bgfx/debug/shaderc,${CMAKE_SOURCE_DIR}/libs/bgfx/release/shaderc> -f "${SHADER_PATH}" -o "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/glsl/${SHADER_NAME}.bin" --type f -p 440 -O 3
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/spirv"
        COMMAND $<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/libs/bgfx/debug/shaderc,${CMAKE_SOURCE_DIR}/libs/bgfx/release/shaderc> -f "${SHADER_PATH}" -o "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/spirv/${SHADER_NAME}.bin" --type f -p spirv16-13 --platform linux -O 3
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/metal"
        COMMAND $<IF:$<CONFIG:Debug>,${CMAKE_SOURCE_DIR}/libs/bgfx/debug/shaderc,${CMAKE_SOURCE_DIR}/libs/bgfx/release/shaderc> -f "${SHADER_PATH}" -o "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/metal/${SHADER_NAME}.bin" --type f -p metal31-14 --platform osx -O 3
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
endforeach()
#----------------------------------