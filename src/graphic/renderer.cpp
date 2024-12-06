#include <bgfx/bgfx.h>
#include <iostream>
#include <bx/string.h>
#include <bx/readerwriter.h>
#include <bx/timer.h>
#include <bx/file.h>
#include <bx/math.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#include <stb/stb_image.h>

#include "renderer.hpp"

Renderer::Renderer() {

}

Renderer::~Renderer() {

}

bool Renderer::tick(){
	return true;
}

bool Renderer::addShape(){
	return true;
}

bool Renderer::removeShape(){
	return true;
}

bool Renderer::addTexture(){
	return true;
}

bool Renderer::removeTexture(){
	return true;
}

bool Renderer::addShader(){
	return true;
}

bool Renderer::removeShader(){
	return true;
}