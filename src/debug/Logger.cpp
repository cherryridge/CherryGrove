#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <thread>
#include <mutex>

#include "Logger.hpp"

typedef uint32_t u32;

using std::cout, std::endl, std::string, std::ofstream, std::mutex, std::to_string, std::stringstream, std::lock_guard, std::thread, std::streambuf;

namespace Logger {
	bool toFile = false;
	ofstream logFile;
	streambuf* consoleBuffer;
	mutex coutMutex;
	thread_local stringstream threadBuffer;
	LoggerCout lout;

	void shutdown() {
		lout << "Terminating logger and reverting cout to console!" << endl;
		cout.rdbuf(consoleBuffer);
		logFile.close();
	}
	
	bool getMode() { return toFile; }

	void setToFile(bool _toFile) {
		if (toFile != _toFile) {
			if (_toFile) {
				consoleBuffer = cout.rdbuf();
				time_t timestamp;
				time(&timestamp);
				string logFileName = "CherryGrove-";
				logFileName += to_string(timestamp);
				logFileName += ".log";
				lout << "Writing log to file " << logFileName << "!" << endl;
				logFile = ofstream(logFileName);
				cout.rdbuf(logFile.rdbuf());
			}
			else {
				lout << "Writing log to console!" << endl;
				cout.rdbuf(consoleBuffer);
			}
			toFile = _toFile;
		}
		else lout << "[Logger] Output not changed." << endl;
	}
}