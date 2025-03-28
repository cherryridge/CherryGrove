#include <filesystem>
#include <ctime>
#include <string>
#include <mutex>
#include <map>

#include "../../debug/debug.hpp"

#include "../file/json/Json.hpp"

#include "saveBase.hpp"
#include "WorldList.hpp"

namespace Save {
	using std::map, std::mutex, std::time_t, std::string;
	using namespace std::filesystem;

	map<time_t, WorldInfo> worldList;
	mutex worldListMutex;

	void refreshWorldList(const char* rootDir) {
		if (!exists(rootDir) || !is_directory(rootDir)) {
			create_directory(rootDir);
			return;
		}
		lout << "[WorldList] Loading saves!" << endl;
		for (const auto& directory : directory_iterator(rootDir)) {
			auto& dirPath = directory.path();
			if (is_regular_file(dirPath)) continue;
			lout << "[WorldList] found directory: " << dirPath << endl;
			string metaPath = dirPath.string() + "\\world.cgb";
			if (!exists(metaPath) || !is_regular_file(metaPath)) continue;
			//todo: Resolve `world.cgb`
		}
	}
}