#pragma once
#include <fstream>

class Logger {
public:
	Logger();
	~Logger();
private:
	std::ofstream logFile;
	std::streambuf* originalBuf;
};