#pragma execution_character_set("utf-8")
#include <filesystem>
#include <string>
#include <locale>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
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
#endif
	typedef int32_t i32;
	using std::filesystem::current_path, std::filesystem::canonical, std::filesystem::remove, std::filesystem::path, std::locale, std::string;

#ifdef _WIN32
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

	locale::global(locale("zh_CN.UTF-8"));

	lout << "Hello from Logger!" << endl;
	lout << "Working directory: " << current_path() << endl;

	lout << "Launching CherryGrove..." << endl;

#ifdef _WIN32
	string lockFilePath = current_path().string();
	lockFilePath += "\\running";
	HANDLE lockFile = CreateFileA(
		lockFilePath.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		nullptr,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	if (lockFile != INVALID_HANDLE_VALUE) {
		bool locked = LockFile(lockFile, 0, 0, 1, 0);
		if (!locked) goto failure;
	}
	else goto failure;
#else
	string lockFilePath = current_path().string();
	lockFilePath += "\\running";
	i32 lockFile = open(lockFilePath.c_str(), O_CREAT | O_RDWR, 0666);
	if (fd >= 0) {
		bool locked = lockf(fd, F_TLOCK, 0) == 0;
		if (!locked) goto failure;
	}
	else goto failure;
#endif // _WIN32

	//CherryGrove, launch!
	CherryGrove::launch();

#ifdef _WIN32
	UnlockFile(lockFile, 0, 0, 1, 0);
	CloseHandle(lockFile);
#else
	lockf(lockFile, F_ULOCK, 0);
	close(lockFile);
#endif // _WIN32
	remove(lockFilePath);

	return 0;

	failure:
	lerr << "[Instance Lock] Please don't launch CherryGrove multiple times from one executable. If you need to use multiple instances, copy the whole directory to a new place, or install again at a different location." << endl;
	return Fatal::MISC_MULTIPLE_INSTANCES;
}