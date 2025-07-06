#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#include "Logger.hpp"

namespace Logger {
    typedef uint32_t u32;
    using std::cout, std::cerr, std::ostream, std::stringstream, std::mutex, std::string, std::streambuf, std::ofstream, std::filesystem::exists, std::filesystem::is_directory, std::filesystem::create_directory, std::to_string;

    bool toFile = false;
    mutex loggerMutex;
    streambuf* coutBuffer;
    streambuf* cerrBuffer;
    ofstream logFile;
    Logger lout(cout), lerr(cerr, "Error", true);

    void setToFile(bool _toFile) noexcept {
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

    void shutdown() noexcept {
        lout << "Terminating logger! (cout will be reverted to console)" << endl;
        cout.rdbuf(coutBuffer);
        logFile.close();
    }
} // namespace Logger