#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <boost/pfr/core.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>

#include "../debug/Logger.hpp"
#include "json_schema/manifest.hpp"
#include "J4mcWrapper.hpp"

using nlohmann::json, std::string, Logger::lout, std::endl, std::ifstream, std::filesystem::file_size, std::filesystem::exists, std::istreambuf_iterator;

namespace J4mcWrapper {
	void test() {
		json j = { {"JSON for Modern C++", "Hello CherryGrove!"} };
		lout << j << endl;
	}
	
	ManifestJSON parseManifest(const char* filePath) {
		if (!exists(filePath)) {
			lout << "Error: JSON not found: " << filePath << endl;
			exit(1);
		}
		ifstream file(filePath);
		if (!file) {
			lout << "Error: Failed to open JSON: " << filePath << endl;
			exit(1);
		}
		string temp((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
		if (!temp.size()) {
			lout << "Error: Failed to read JSON: " << filePath << endl;
			exit(1);
		}
		json result = json::parse(temp, nullptr, true, true);
		lout << "ns: " << result["nameSpace"] << endl;
		return {};
	}

	template<typename T>
	void parse(const char* filePath, T& targetStruct) {

	}

	static json& getJSON(const char* filePath) {
		if (!exists(filePath)) throw ERROR_FILE_NOT_EXIST;
		ifstream file(filePath);
		if (!file) throw ERROR_CANNOT_OPEN_FILE;
		string temp((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
		if (!temp.size()) {
			lout << "Error: Failed to read JSON: " << filePath << endl;
			exit(1);
		}
	}
}