cmake_minimum_required(VERSION 3.10.0)

project(
    CherryGrove
    VERSION 0.1.0
    LANGUAGES CXX
)

add_executable(CherryGrove src/main.cpp)

target_include_directories(CherryGrove PRIVATE include)

target_link_libraries(CherryGrove PRIVATE Opengl32.lib)
target_link_libraries(CherryGrove PRIVATE ${CMAKE_SOURCE_DIR}/libs/GLEW/glew32s.lib)
target_link_libraries(CherryGrove PRIVATE ${CMAKE_SOURCE_DIR}/libs/GLFW/glfw3.lib)

target_compile_features(CherryGrove PUBLIC cxx_std_17)