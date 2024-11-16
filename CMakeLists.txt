set(ENV{LANG} "en_US.UTF-8")

cmake_minimum_required(VERSION 3.30)

project(CherryGrove VERSION 0.0.1 LANGUAGES CXX)

find_package(GLEW CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(FTGL CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(CherryGrove ${SOURCES})

target_link_libraries(CherryGrove PRIVATE GLEW::GLEW)
target_link_libraries(CherryGrove PRIVATE Opengl32.lib)
target_link_libraries(CherryGrove PRIVATE glfw)
target_link_libraries(CherryGrove PRIVATE ftgl)

target_compile_features(CherryGrove PUBLIC cxx_std_23)
target_sources(CherryGrove PRIVATE meta.rc)

target_link_options(CherryGrove PRIVATE
    $<$<CONFIG:Release>:-mwindows>
    $<$<CONFIG:Debug>:-mconsole>
)

set(MINGW_BIN "$ENV{MINGW_HOME}/bin")
string(REPLACE "\\" "/" MINGW_BIN "${MINGW_BIN}")
install(
    TARGETS CherryGrove DESTINATION .
    RUNTIME_DEPENDENCIES
        PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-" "hvsifiletrust"
        POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
        DIRECTORIES ${CMAKE_BINARY_DIR} ${MINGW_BIN}
    RUNTIME DESTINATION .
)
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "CherryGrove")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
#set(CPACK_PACKAGE_CONTACT "")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Cherry Grove Engine")
set(CPACK_PACKAGE_VENDOR "LJM12914")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CherryGrove")
set(CPACK_NSIS_DISPLAY_NAME "CherryGrove")
set(CPACK_NSIS_EXECUTABLES_DIRECTORY .)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
#set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$DESKTOP\\\\CherryGrove.lnk' '$INSTDIR\\\\CherryGrove.exe'")

include(CPack)