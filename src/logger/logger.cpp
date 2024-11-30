#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include "logger.hpp"

Logger::Logger() {
	originalBuf = std::cout.rdbuf();
	time_t timestamp;
	time(&timestamp);
	std::string logFileName = "CherryGrove-";
	logFileName.append(std::to_string(timestamp));
	logFileName.append(".log");
	std::cout << "Writing log to " << logFileName << "!" << std::endl;
	logFile = std::ofstream(logFileName);
	std::cout.rdbuf(logFile.rdbuf());
}

Logger::~Logger() {
	std::cout << "Closing log file!" << std::endl;
	std::cout.rdbuf(originalBuf);
	logFile.close();
}