#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "Logger.hpp"

namespace Logger {
    typedef uint32_t u32;

    using std::cout, std::cerr, std::endl, std::string, std::ofstream, std::mutex, std::to_string, std::stringstream, std::streambuf, std::filesystem::exists, std::filesystem::is_directory, std::filesystem::create_directory, std::thread;

    bool toFile = false;
    mutex loggerMutex;
    ofstream logFile;
    streambuf* coutBuffer;
    streambuf* cerrBuffer;
    thread_local stringstream threadBufferOdi;
    LoggerCout lout;
    thread_local stringstream threadBufferErr;
    LoggerCerr lerr;
    unordered_map<thread::id, string> threadNames;

    void shutdown() {
        lout << "Terminating logger! (cout will be reverted to console)" << endl;
        cout.rdbuf(coutBuffer);
        logFile.close();
    }
    
    bool getMode() { return toFile; }

    void setToFile(bool _toFile) {
        if (toFile != _toFile) {
            if (_toFile) {
                coutBuffer = cout.rdbuf();
                cerrBuffer = cerr.rdbuf();
                time_t timestamp;
                time(&timestamp);
                if (!exists("logs") || !is_directory("logs")) {
                    if (!create_directory("logs")) {
                        lerr << "[Logger] Failed to create /logs directory!" << endl;
                        //Refuse to redirect logs to prevent data loss.
                        return;
                    }
                }
                string logFileName = "logs/CherryGrove-";
                logFileName += to_string(timestamp);
                logFileName += ".log";
                logFile = ofstream(logFileName);
                if (!logFile.is_open()) {
                    lerr << "[Logger] Failed to open log file: " << logFileName << "\n";
                    //Refuse to redirect logs to prevent data loss.
                    return;
                }
                else {
                    lout << "Writing log to " << logFileName << "!" << endl;
                    cout.rdbuf(logFile.rdbuf());
                    cerr.rdbuf(logFile.rdbuf());
                }
            }
            else {
                lout << "Writing log to console!" << endl;
                cout.rdbuf(coutBuffer);
                cerr.rdbuf(cerrBuffer);
            }
            toFile = _toFile;
        }
        else lout << "Logger output not changed." << endl;
    }
}