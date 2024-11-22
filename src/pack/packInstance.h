#pragma once
#include "pack.h"

class PackInstance {
	PackInstance(const char* folderPath);
private:
	PackConfig config;
};