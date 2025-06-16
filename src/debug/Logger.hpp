﻿#pragma once
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <boost/unordered/unordered_flat_map.hpp>

//Auto use magic variables
using std::endl, std::flush;

namespace Logger {
    using std::cout, std::cerr, std::ostream, std::enable_if, std::is_function, std::stringstream, std::this_thread::get_id, std::lock_guard, std::mutex, std::string, std::thread, std::lock_guard, boost::unordered::unordered_flat_map, std::streambuf, std::ofstream, std::filesystem::exists, std::filesystem::is_regular_file, std::filesystem::is_directory, std::filesystem::create_directory, std::to_string;

    inline bool toFile = false;
    inline static mutex loggerMutex;
    inline static unordered_flat_map<thread::id, string, std::hash<thread::id>> threadNames;
    inline static streambuf* coutBuffer;
    inline static streambuf* cerrBuffer;
    inline static ofstream logFile;

    inline static thread_local stringstream threadBufferOdi;
    struct LoggerCout {
        LoggerCout& operator<<(ostream& (*manip)(ostream&)) {
            if (manip == static_cast<ostream& (*)(ostream&)>(endl)) {
                //Use l* << <Content> << endl to output thread-safe content.
                //l* << endl will result in nothing.
                if (!threadBufferOdi.str().empty()) {
                    lock_guard<mutex> lock(loggerMutex);
                    cout << threadBufferOdi.str() << endl << flush;
                    threadBufferOdi.str("");
                    threadBufferOdi.clear();
                }
            }
            else if (manip == static_cast<ostream& (*)(ostream&)>(flush)) {
                //Use l* << <Name> << flush to set a customized name for this thread.
                if (!threadBufferOdi.str().empty()) {
                    lock_guard<mutex> lock(loggerMutex);
                    string tBstr = threadBufferOdi.str(), name = tBstr.substr(tBstr.find_first_of(' ') + 1);
                    auto p = threadNames.find(get_id());
                    if (p == threadNames.end()) threadNames.emplace(get_id(), name);
                    else p->second = name;
                    threadBufferOdi.str("");
                    threadBufferOdi.clear();
                }
                //Use l* << flush to delete the customized thread name.
                else {
                    auto p = threadNames.find(get_id());
                    if (p != threadNames.end()) threadNames.erase(p);
                }
            }
            else {
                if (threadBufferOdi.str().empty()) {
                    auto p = threadNames.find(get_id());
                    if (p == threadNames.end()) threadBufferOdi << "[" << get_id() << "] ";
                    else threadBufferOdi << "[" << p->second << "] ";
                }
                threadBufferOdi << manip;
            }
            return *this;
        }

        template <typename T>
        typename enable_if<!is_function<T>::value, LoggerCout&>::type operator<<(const T& value) {
            if (threadBufferOdi.str().empty()) {
                auto p = threadNames.find(get_id());
                if (p == threadNames.end()) threadBufferOdi << "[" << get_id() << "] ";
                else threadBufferOdi << "[" << p->second << "] ";
            }
            threadBufferOdi << value;
            return *this;
        }

    };
    inline LoggerCout lout;

    inline static thread_local stringstream threadBufferErr;
    struct LoggerCerr {
        LoggerCerr& operator<<(ostream& (*manip)(ostream&)) {
            if (manip == static_cast<ostream & (*)(ostream&)>(endl)) {
                //Use l* << <Content> << endl to output thread-safe content.
                //l* << endl will result in nothing.
                if (!threadBufferErr.str().empty()) {
                    lock_guard<mutex> guard(loggerMutex);
                    //No need to flush because `cerr` will automatically empty the buffer to the screen.
                    cerr << threadBufferErr.str() << endl;
                    threadBufferErr.str("");
                    threadBufferErr.clear();
                }
            }
            else if (manip == static_cast<ostream & (*)(ostream&)>(flush)) {
                //Use l* << <Name> << flush to set a customized name for this thread.
                if (!threadBufferErr.str().empty()) {
                    lock_guard<mutex> guard(loggerMutex);
                    string tBstr = threadBufferErr.str(), name = tBstr.substr(tBstr.find_first_of(' ') + 1);
                    auto p = threadNames.find(get_id());
                    if (p == threadNames.end()) threadNames.emplace(get_id(), name);
                    else p->second = name;
                    threadBufferErr.str("");
                    threadBufferErr.clear();
                }
                //Use l* << flush to delete the customized thread name.
                else {
                    auto p = threadNames.find(get_id());
                    if (p != threadNames.end()) threadNames.erase(p);
                }
            }
            else {
                if (threadBufferErr.str().empty()) {
                    auto p = threadNames.find(get_id());
                    if (p == threadNames.end()) threadBufferErr << "[" << get_id() << "] ";
                    else threadBufferErr << "[" << p->second << "] ";
                }
                threadBufferErr << manip;
            }
            return *this;
        }

        template <typename T>
        typename enable_if<!is_function<T>::value, LoggerCerr&>::type operator<<(const T& value) {
            if (threadBufferErr.str().empty()) {
                auto p = threadNames.find(get_id());
                if (p == threadNames.end()) threadBufferErr << "[" << get_id() << "][Error] ";
                else threadBufferErr << "[" << p->second << "][Error] ";
            }
            threadBufferErr << value;
            return *this;
        }

    };
    inline LoggerCerr lerr;

    inline void shutdown() noexcept {
        lout << "Terminating logger! (cout will be reverted to console)" << endl;
        cout.rdbuf(coutBuffer);
        logFile.close();
    }
    
    inline void setToFile(bool _toFile) noexcept {
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

//Auto use magic variables
using Logger::lout, Logger::lerr;