#include <filesystem>
#include <string>

#include "../debug/debug.hpp"

#include "FileDrop.hpp"

namespace FileDrop {
	typedef int32_t i32;
	using namespace std::filesystem;
	using std::string;

	//CherryGrove World
	static void processCGW(const path& _path){
		
	}

	//May be world or pack
	static void processFolder(const path& _path) {

	}

	//May be world or pack
	static void processZip(const path& _path) {

	}

	//CherryGrove Pack
	static void processCGP(const path& _path) {

	}

	void processFile(i32 count, const char** paths) {
		for (i32 i = 0; i < count; i++) {
			std::string pathStr(paths[i]);
			path _path(pathStr);
			auto extension =  _path.extension().string();
			if (extension == ".cgw") processCGW(_path);
			else if (extension == ".zip") processZip(_path);
			else if (extension == ".cgp") processCGP(_path);
			else if (is_directory(_path)) processFolder(_path);
			else lout << "Unidentified file type: " << pathStr << endl;
		}
	}
}