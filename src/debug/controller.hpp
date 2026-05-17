#pragma once
#include <ctime>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

#include "AtomicizedOutput.hpp"
#include "LoggingMode.hpp"
#include "loggers.hpp"

namespace Debug {
    typedef int64_t i64;
    using std::cout, std::cerr, std::endl, std::ios, std::string, std::ofstream, std::filesystem::exists, std::filesystem::is_directory, std::filesystem::create_directory, std::format;

    namespace detail {
        inline ofstream logFile;
        inline AtomicizedOutput atomicLogFile{logFile};
    }

    inline void init(LoggingMode mode) noexcept {
        switch(mode) {
            //The two logger object use stdout by default. Nothing to do here.
            case LoggingMode::Stdout: break;
            case LoggingMode::Separate:
            #if CG_PLATFORM_WINDOWS
            {
                AllocConsole();
                SetConsoleOutputCP(CP_UTF8);
                SetConsoleCP(CP_UTF8);
                FILE* fDummy = nullptr;
                freopen_s(&fDummy, "CONIN$", "r", stdin);
                freopen_s(&fDummy, "CONOUT$", "w", stdout);
                freopen_s(&fDummy, "CONOUT$", "w", stderr);
                ios::sync_with_stdio();
                cout.clear();
                cerr.clear();
                break;
            }
            //todo: Do we need to do anything in Linux or macOS?
            #elif CG_PLATFORM_LINUX
                break;
            #elif CG_PLATFORM_MACOS
                break;
            #elif CG_PLATFORM_ANDROID || CG_PLATFORM_IOS
                cout << "We can't spawn console on mobile, falling back to file output." << endl;
                [[fallthrough]];
            #endif
            case LoggingMode::File: {
                if ((!exists("logs") || !is_directory("logs")) && !create_directory("logs")) {
                    cerr << "(Error)[Logger] Failed to create /logs directory!" << endl;
                    //Refuse to redirect logs to prevent data loss.
                    return;
                }
                time_t timestamp;
                time(&timestamp);
                string logFileName = format("logs/CherryGrove-{}.log", static_cast<i64>(timestamp));
                detail::logFile = ofstream(logFileName);
                if (!detail::logFile.is_open()) {
                    cerr << "(Error)[Logger] Failed to open log file: " << logFileName << "\n";
                    //Refuse to redirect logs to prevent data loss.
                    return;
                }
                else {
                    cout << "Writing log to " << logFileName << "!" << endl;
                    lout.redirectOutput(&detail::atomicLogFile);
                    lerr.redirectOutput(&detail::atomicLogFile);
                }
                break;
            }
        }
    }

    inline void shutdown() noexcept {
        lout << "Terminating logger!" << nlaf;
        if (detail::logFile.is_open()) detail::logFile.close();
    }
}