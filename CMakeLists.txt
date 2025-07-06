cmake_minimum_required(VERSION "3.20")

project("CherryGrove")

#------------Basic-----------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)
set(CMAKE_SUPPRESS_REGENERATION ON CACHE BOOL "" FORCE)
#----------------------------------

#------------MSVC------------------
if(MSVC)
    add_link_options(/INCREMENTAL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Zc:preprocessor /utf-8")
    #Some C deps use `/source-charset:utf-8` and some use `/utf-8`, so we're not going to bother with it.
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    cmake_policy(SET CMP0091 NEW)
    add_compile_definitions(
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()
#----------------------------------

#------------Clangd----------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
#----------------------------------

#------------Files-----------------
file(GLOB_RECURSE CG_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE IMGUI_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/libs/imgui/*.cpp")
#----------------------------------

#------------Dep:Prebuilt----------
set(BGFX_BGFX_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bgfx$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")
set(BGFX_BX_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bx$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")
set(BGFX_BIMG_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bimg$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")

set(LUAJIT_LIB "${CMAKE_SOURCE_DIR}/libs/luajit/$<IF:$<CONFIG:Debug>,debug,release>/$<IF:$<PLATFORM_ID:Windows>,lua51.lib,libluajit.a>")

set(SOLOUD_LIB "${CMAKE_SOURCE_DIR}/libs/soloud/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>soloud_static_x64$<$<CONFIG:Debug>:_d>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")

set(V8_LIB "${CMAKE_SOURCE_DIR}/libs/v8/$<IF:$<CONFIG:Debug>,debug,release>/$<IF:$<PLATFORM_ID:Windows>,v8_monolith.lib,libv8_monolith.a>")
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
add_subdirectory("libs/physfs")

set(WITH_LZ4                    ON  CACHE BOOL "")
set(WITH_ZSTD                   ON  CACHE BOOL "")
set(WITH_WINDOWS_UTF8_FILENAMES ON  CACHE BOOL "")
set(ROCKSDB_BUILD_SHARED        OFF CACHE BOOL "")
set(WITH_MD_LIBRARY             OFF CACHE BOOL "")
set(WITH_TESTS                  OFF CACHE BOOL "")
set(WITH_BENCHMARK_TOOLS        OFF CACHE BOOL "")
set(WITH_TOOLS                  OFF CACHE BOOL "")
set(WITH_CORE_TOOLS             OFF CACHE BOOL "")
set(WITH_ALL_TESTS              OFF CACHE BOOL "")
set(WITH_BENCHMARK_TOOLS        OFF CACHE BOOL "")
#What the fuck does this option exist?
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(WITH_RUNTIME_DEBUG ON  CACHE BOOL "")
else()
    set(WITH_RUNTIME_DEBUG OFF CACHE BOOL "")
endif()
add_subdirectory("libs/rocksdb")

set(FT_DISABLE_ZLIB OFF CACHE BOOL "")
add_subdirectory("libs/freetype")

set(ZLIB_ENABLE_TESTS   OFF CACHE BOOL "")
set(ZLIBNG_ENABLE_TESTS OFF CACHE BOOL "")
set(WITH_GTEST          OFF CACHE BOOL "")
set(ZLIB_COMPAT         ON  CACHE BOOL "")
add_subdirectory("libs/zlib_ng")

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

#For json_schema_validator.
add_subdirectory("libs/json")

set(ENV{NLOHMANN_JSON_VERSION} "v3.12.0")
set(nlohmann_json_VERSION "v3.12.0")
add_subdirectory("libs/json_schema_validator")
#----------------------------------

#------------Dep:Internal----------
set(CGNBT_HAS_ZLIBNG ON CACHE BOOL "")
add_subdirectory("libs/cgnbt")
#----------------------------------

#------------Dep:System------------
if(UNIX AND NOT APPLE)
    find_package(ALSA REQUIRED)
    find_package(PkgConfig REQUIRED)
    find_package(Threads REQUIRED)
    pkg_check_modules(JACK REQUIRED jack)
elseif(APPLE)
    find_library(AUDIO_TOOLBOX_LIBRARY AudioToolbox)
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(FOUNDATION_LIBRARY Foundation)
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
endif()
#----------------------------------

#------------CherryGrove-----------
add_executable(${PROJECT_NAME} ${CG_SOURCES} ${IMGUI_SOURCES})
#----------------------------------

#------------Build:Marco-----------
target_compile_definitions(${PROJECT_NAME} PRIVATE $<IF:$<CONFIG:Debug>,DEBUG,NDEBUG>)
target_compile_definitions(${PROJECT_NAME} PRIVATE "BX_CONFIG_DEBUG=$<IF:$<CONFIG:Debug>,1,0>")
#----------------------------------

#------------Inclusions------------
target_include_directories(${PROJECT_NAME} PRIVATE
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
    "libs/entt/single_include"
    "libs/glm"
    "libs/json/single_include"

    "libs/physfs/src"
    "lib/rocksdb/include"
    "libs/freetype/include"
    "libs/sdl/include"
    "libs/sdl_image/include"
    "libs/json_schema_validator/src"
    "libs/zlib_ng"

    "libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/include"
    "libs/luajit/$<IF:$<CONFIG:Debug>,debug,release>/include"
    "libs/soloud/$<IF:$<CONFIG:Debug>,debug,release>/include"
    "libs/v8/$<IF:$<CONFIG:Debug>,debug,release>/include"

    "libs/cgnbt/include"

    "libs/imgui"
)
#----------------------------------

#------------Link------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
    CGNBT

    physfs-static
    freetype
    SDL3-static
    SDL3_image-static
    nlohmann_json_schema_validator
    zlibstatic

    ${BGFX_BGFX_LIB}
    ${BGFX_BX_LIB}
    ${BGFX_BIMG_LIB}
    ${LUAJIT_LIB}
    ${SOLOUD_LIB}
    ${V8_LIB}
)
#----------------------------------

#------------Link:System-----------
if(WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
    target_link_libraries(${PROJECT_NAME} PRIVATE dbghelp)
    target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/packing/windows")
    target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/packing/windows/CherryGrove.rc")
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${ALSA_LIBRARIES}
        ${JACK_LIBRARIES}
        Threads::Threads
        dl
    )
elseif(APPLE)
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
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets"  "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/packs")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/packs"  "$<TARGET_FILE_DIR:${PROJECT_NAME}>/packs")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/saves")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/saves"  "$<TARGET_FILE_DIR:${PROJECT_NAME}>/saves")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/test")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/test"  "$<TARGET_FILE_DIR:${PROJECT_NAME}>/test")
endif()
if(EXISTS "${CMAKE_SOURCE_DIR}/options.json")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/options.json"  "$<TARGET_FILE_DIR:${PROJECT_NAME}>/options.json")
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