#pragma once
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <CLI/App.hpp>
#include <CLI/CLI.hpp>

#include "../debug/Fatal.hpp"
#include "../meta.hpp"
#include "../util/os/filesystem.hpp"
#include "../util/os/platform.hpp"
#include "getExecutableDirectory.hpp"

namespace Boot {
    typedef uint32_t u32;
    typedef int64_t i64;
    using std::string, std::vector, std::cout, std::cerr, std::endl, CLI::App, CLI::ParseError, CLI::CallForHelp, CLI::CallForVersion, CLI::AppFormatMode, Util::OS::isWritableDirectory, Util::OS::normalize, std::filesystem::current_path, std::filesystem::path;

    //Note: Pre-logger function
    //Note: The very first function called within Main::launch, the entry point after OS-specific entry.
    inline void setWorkingDirectory(int argc, char** argv) noexcept {
        //Regain I/O of parent console.
        //Warning: /SUBSYSTEM:WINDOWS will cause PowerShell and Command Prompt to run asynchronously and ignore our console output. Currently, the best solution is to tell users to use `.\CherryGrove.exe -h | Out-Null` in PowerShell, or `start "" /b /wait .\CherryGrove.exe -h` in Command Prompt to wait for the process to finish IN THE DOCUMENTATION.
        #if CG_PLATFORM_WINDOWS
            if (AttachConsole(ATTACH_PARENT_PROCESS) || GetLastError() == ERROR_ACCESS_DENIED) {
                FILE* fp = nullptr;
                freopen_s(&fp, "CONIN$", "r", stdin);
                freopen_s(&fp, "CONOUT$", "w", stdout);
                freopen_s(&fp, "CONOUT$", "w", stderr);
                SetConsoleOutputCP(CP_UTF8);
                SetConsoleCP(CP_UTF8);
                setvbuf(stdout, nullptr, _IONBF, 0);
                setvbuf(stderr, nullptr, _IONBF, 0);
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                if (hOut && hOut != INVALID_HANDLE_VALUE) {
                    DWORD mode = 0;
                    if (GetConsoleMode(hOut, &mode)) SetConsoleMode(hOut, mode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
                    CONSOLE_SCREEN_BUFFER_INFO csbi{};
                    if (GetConsoleScreenBufferInfo(hOut, &csbi) && csbi.dwCursorPosition.X != 0) cout << '\n';
                }
            }
        #endif

        App app;
        app.allow_windows_style_options(false);
        app.allow_config_extras(false);
        app.set_help_all_flag("");

        app.set_version_flag("-v, --version", CG_SEMATIC_VERSION, "Display sematic version and exit.");

        app.add_flag("-e,--engine", [](i64) {
            cout << CG_ENGINE_VERSION << endl;
            exit(0);
        }, "Display (integer) engine version and exit.");

        app.set_help_flag("-h,--help", "Print help message and exit.");

        string generateKeyword;
        app.add_option("-g,--generate", "Generate something instead of launching the game.")->type_name("[KEYWORD]")->check(CLI::IsMember({"schemas", "config"}));

        string workingDirectory;
        app.add_option("WorkingDirectory", workingDirectory, "(Optional) CherryGrove's working directory.")->check(CLI::ExistingPath);

        app.footer(string("Use the default config file by executing CherryGrove without any arguments.\nFor more information, please visit https://docs.cherrygrove.dev.\n\nCherryGrove is source-available software because it's a shame of open source to allow unrewarded commercial use.\n") + CG_COPYRIGHT_NOTICE + "\nhttps://cherrygrove.dev");

        try { app.parse(argc, argv); }
        catch (const CallForHelp&) {
            cout << app.help("", AppFormatMode::All) << endl;
            exit(0);
        }
        catch (const CallForVersion&) {
            cout << CG_SEMATIC_VERSION << endl;
            exit(0);
        }
        catch (const ParseError& e) {
            cerr << "[Error][CLI] Error occured during argument parsing: (" << e.get_exit_code() << ") " << e.get_name() << " " << e.what() << endl;
            exit(Fatal::BOOT_INVALID_WORKING_DIR);
        }

        if (generateKeyword == "schemas") {
            cout << "TODO: Dump schemas" << endl;
            exit(0);
        }
        else if (generateKeyword == "config") {
            cout << "TODO: Generate default config" << endl;
            exit(0);
        }

        if (workingDirectory.empty()) {
            const auto executablePath = getExecutableDirectory();
            if (!isWritableDirectory(executablePath)) {
                //todo: OS-specific diversions
                //1. Linux tends to put app executable in a read-only directory, according to plan, in this scenario we will:
                //  a. Try to use PWD. PWD IS NOT SET TO EXECUTABLE DIRECTORY YET. THIS IS LITERALLY THE SECOND FUNCTION FROM THE ENTRYPOINT.
                //  b. Use the global storage via XDG.
                //  c. Fail.
                //2. Research for MacOS common practices.
                cerr << "[Error][CLI] Current working directory is not writable. TODO: OS-specific diversions" << endl;
                Fatal::exit(Fatal::FILESYSTEM_NO_WRITE_PERMISSION);
            }
            cout << "[CLI] No working directory specified, using executable directory: " << executablePath << endl;
            workingDirectory = executablePath.string();
        }
        else if (!isWritableDirectory(workingDirectory)) {
            cerr << "[Error][CLI] Specified working directory is not writable: " << workingDirectory << endl;
            Fatal::exit(Fatal::FILESYSTEM_NO_WRITE_PERMISSION);
        }

        path workingDirectoryPath(workingDirectory);
        if (!normalize(workingDirectoryPath)) {
            cerr << "[Error][Boot] Invalid working directory path: " << workingDirectoryPath << endl;
            Fatal::exit(Fatal::BOOT_INVALID_WORKING_DIR);
        }
        current_path(workingDirectoryPath);
    }
}