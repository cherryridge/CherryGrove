#pragma once
#include "pack.hpp"

class PackInstance {
	PackInstance(const char* folderPath);
private:
	PackConfig config;
};