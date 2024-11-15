cmake_minimum_required(VERSION 3.12)

project(CherryGrove)

find_package(GLEW CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(FTGL CONFIG REQUIRED)

add_executable(CherryGrove src/main.cpp)

target_link_libraries(CherryGrove PRIVATE GLEW::GLEW)
target_link_libraries(CherryGrove PRIVATE Opengl32.lib)
target_link_libraries(CherryGrove PRIVATE glfw)
target_link_libraries(CherryGrove PRIVATE ftgl)

target_compile_features(CherryGrove PUBLIC cxx_std_17)