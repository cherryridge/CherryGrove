cmake_minimum_required(VERSION "3.20")

project("CherryGrove")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0091 NEW)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Zc:preprocessor")
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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

file(GLOB_RECURSE CG_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE IMGUI_SOURCES CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/libs/imgui/*.cpp")

set(BGFX_BGFX_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bgfx$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")
set(BGFX_BX_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bx$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")
set(BGFX_BIMG_LIB "${CMAKE_SOURCE_DIR}/libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>bimg$<IF:$<CONFIG:Debug>,Debug,Release>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")

set(SOLOUD_LIB "${CMAKE_SOURCE_DIR}/libs/soloud/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>soloud_static_x64$<$<CONFIG:Debug>:_d>$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")

set(V8_LIB "${CMAKE_SOURCE_DIR}/libs/v8/$<IF:$<CONFIG:Debug>,debug,release>/$<$<NOT:$<PLATFORM_ID:Windows>>:lib>v8_monolith$<IF:$<PLATFORM_ID:Windows>,.lib,.a>")

add_subdirectory("libs/freetype")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
add_subdirectory("libs/glfw")
add_subdirectory("libs/json")
set(ENV{NLOHMANN_JSON_VERSION} "v3.12.0")
set(nlohmann_json_VERSION "v3.12.0")
add_subdirectory("libs/json_schema_validator")
set(ZLIB_ENABLE_TESTS   OFF CACHE BOOL "")
set(ZLIBNG_ENABLE_TESTS OFF CACHE BOOL "")
set(WITH_GTEST          OFF CACHE BOOL "")
add_subdirectory("libs/zlib_ng")

set(ZLIBNG_ROOT "${CMAKE_SOURCE_DIR}/libs/zlib-ng" CACHE PATH "")
add_subdirectory("libs/cgnbt")

if(UNIX AND NOT APPLE)
    find_package(ALSA REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(JACK REQUIRED jack)
    find_package(Threads REQUIRED)
elseif(APPLE)
    find_library(COREAUDIO_LIBRARY CoreAudio)
    find_library(AUDIO_TOOLBOX_LIBRARY AudioToolbox)
    find_library(METAL_LIBRARY Metal)
    find_library(FOUNDATION_LIBRARY Foundation)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
endif()

add_executable(${PROJECT_NAME} ${CG_SOURCES} ${IMGUI_SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE $<IF:$<CONFIG:Debug>,DEBUG,NDEBUG>)
target_compile_definitions(${PROJECT_NAME} PRIVATE "BX_CONFIG_DEBUG=$<IF:$<CONFIG:Debug>,1,0>")

target_include_directories(${PROJECT_NAME} PRIVATE
    "libs/boost/assert/include"
    "libs/boost/config/include"
    "libs/boost/detail/include"
    "libs/boost/exception/include"
    "libs/boost/pfr/include"
    "libs/boost/throw_exception/include"
    "libs/boost/type_traits/include"
    "libs/boost/utility/include"
    "libs/boost/uuid/include"
    "libs/entt/single_include"
    "libs/glm"
    "libs/json/single_include"
    "libs/stb"

    "libs/cgnbt/include"

    "libs/freetype/include"
    "libs/glfw/include"
    "libs/json_schema_validator/src"
    "libs/zlib_ng"

    "libs/bgfx/$<IF:$<CONFIG:Debug>,debug,release>/include"
    "libs/soloud/$<IF:$<CONFIG:Debug>,debug,release>/include"
    "libs/v8/$<IF:$<CONFIG:Debug>,debug,release>/include"

    "libs/imgui"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    freetype
    glfw
    nlohmann_json_schema_validator
    zlibstatic

    ${BGFX_BGFX_LIB}
    ${BGFX_BX_LIB}
    ${BGFX_BIMG_LIB}
    ${SOLOUD_LIB}
    ${V8_LIB}
)

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