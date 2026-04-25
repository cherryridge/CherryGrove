#pragma once
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>
#include <CLI/App.hpp>
#include <CLI/CLI.hpp>

#include "../debug/Fatal.hpp"
#include "../meta.hpp"
#include "../settings/Settings.hpp"
#include "../util/json/generateSchemas.hpp"
#include "../util/os/filesystem.hpp"
#include "../util/os/platform.hpp"
#include "getExecutableDirectory.hpp"

namespace Boot {
    typedef int64_t i64;
    using std::string, std::vector, std::filesystem::path, std::cout, std::cerr, std::endl, std::exception, std::filesystem::current_path, std::error_code, CLI::App, CLI::ParseError, CLI::CallForHelp, CLI::CallForVersion, CLI::AppFormatMode, CLI::to_path, Util::OS::isWritableDirectory, Util::OS::normalize, Util::OS::getU8String;

    //note: Pre-logger function
    //note: The very first function called within Main::launch, the entry point after OS-specific entry.
    inline void setWorkingDirectory(int argc, char** argv) noexcept {
        //Regain I/O of parent console.
        //warning: /subsystem:windows will cause PowerShell and Command Prompt to run asynchronously and ignore our console output. Currently, the best solution is to tell users to use `.\CherryGrove.exe -h | Out-Null` in PowerShell, or `start "" /b /wait .\CherryGrove.exe -h` in Command Prompt to wait for the process to finish IN THE DOCUMENTATION.
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
        }, "Display canonical engine version and exit.");

        app.set_help_flag("-h,--help", "Print help message and exit.");

        string generateKeyword;
        app.add_option("-g,--generate", generateKeyword, "Generate something (at the working directory) instead of launching the game.\n`schema`: Generate JSON schema files.\n`settings`: Generate the default `settings.json` file.")->type_name("[KEYWORD]")->check(CLI::IsMember({"schema", "settings"}));

        string workingDirectory;
        app.add_option("WorkingDirectory", workingDirectory, "CherryGrove's working directory.\nOptional. If not provided, CherryGrove will use the executable's directory.")->check(CLI::ExistingPath);
        
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
            cerr << "(Error) [CLI] Error occured during argument parsing: (" << e.get_exit_code() << ") " << e.get_name() << " " << e.what() << endl;
            exit(Fatal::BOOT_INVALID_WORKING_DIR);
        }
        
        if (generateKeyword == "schema") {
            Util::Json::generateSchemas();
            cout << "Schemas generated." << endl;
            exit(0);
        }
        else if (generateKeyword == "settings") {
            if (!Settings::saveSettings()) cerr << "(Error) [Boot] Failed to generate default settings." << endl;
            else cout << "Default settings generated." << endl;
            exit(0);
        }
        
        path workingDirectoryPath;
        if (workingDirectory.empty()) {
            const auto executablePath = getExecutableDirectory();
            if (isWritableDirectory(executablePath)) {
                cout << "[CLI] No working directory specified, using executable directory: " << getU8String(executablePath) << endl;
                workingDirectoryPath = executablePath;
            }
            else {
                cout << "[CLI] Executable directory is not writable. Trying to use PWD." << endl;
                error_code ec;
                const path currentPath = current_path(ec);
                if (ec || !isWritableDirectory(currentPath)) {
                    cerr << "(Error) [CLI] Both executable directory and current working directory are not writable. CherryGrove needs to have a writable working directory. If you're using Linux or macOS, consider providing the working directory argument: `./CherryGrove <path-to-wd>`." << endl;
                    Fatal::exit(Fatal::FILESYSTEM_NO_WRITE_PERMISSION);
                }
                cout << "[CLI] No working directory specified, using current working directory: " << getU8String(currentPath) << endl;
                workingDirectoryPath = currentPath;
            }
        }
        else {
            try { workingDirectoryPath = to_path(workingDirectory); }
            catch (const exception& e) {
                cerr << "(Error) [Boot] Invalid working directory path: " << workingDirectory << " (" << e.what() << ")" << endl;
                Fatal::exit(Fatal::BOOT_INVALID_WORKING_DIR);
            }
            if (!isWritableDirectory(workingDirectoryPath)) {
                cerr << "(Error) [CLI] Specified working directory is not writable: " << workingDirectory << endl;
                Fatal::exit(Fatal::FILESYSTEM_NO_WRITE_PERMISSION);
            }
        }

        if (!normalize(workingDirectoryPath)) {
            cerr << "(Error) [Boot] Invalid working directory path: " << getU8String(workingDirectoryPath) << endl;
            Fatal::exit(Fatal::BOOT_INVALID_WORKING_DIR);
        }

        error_code ec;
        current_path(workingDirectoryPath, ec);
        if (ec) {
            cerr << "(Error) [Boot] Failed to set working directory path: " << getU8String(workingDirectoryPath) << " (" << ec.message() << ")" << endl;
            Fatal::exit(Fatal::BOOT_SET_WORKING_DIR_FAILED);
        }
    }
}