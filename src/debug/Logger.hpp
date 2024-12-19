#pragma once
#include <fstream>

using std::ofstream, std::streambuf;

class Logger {
public:
	Logger();
	~Logger();
private:
	ofstream logFile;
	streambuf* originalBuf;
};