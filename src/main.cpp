#pragma execution_character_set("utf-8")
#include <filesystem>
#if _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include "debug/debug.hpp"
#include "CherryGrove.hpp"

//Whether the program is running as a console program or a Win32 window program.
#ifdef CG_CONSOLE
int main(int argc, char* argv[]) {
	char** _argv = argv;
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	char** _argv = __argv;
#endif //CG_CONSOLE

	using std::filesystem::current_path, std::filesystem::canonical, std::filesystem::path;

#if _WIN32
	if (!IsDebuggerPresent()) {
		path exePath(_argv[0]);
		path exeDir = canonical(exePath).parent_path();
		current_path(exeDir);
	}
#endif
	std::cout << "Setting up logger..." << endl;

#ifndef CG_CONSOLE
	Logger::setToFile(true);
#endif

#ifdef _DEBUG
	if (IsDebuggerPresent()) {
		//Hacks for auto compiling shaders every time the program starts, while Visual Studio build events does not trigger when nothing in the main code has changed since last build.
		//Sadly only for Visual Studio debugging.
		//lout << "Compiling shaders at runtime for debug..." << endl;
		//current_path("scripts");
		//system("compile_shaders.bat");
		//current_path("..");
	}
#endif //_DEBUG

	lout << "Hello from Logger!" << endl;
	lout << "Working directory: " << current_path() << endl;

	lout << "Launching CherryGrove..." << endl;
	CherryGrove::launch();
}