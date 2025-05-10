#include <string>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <exception>
#include <vector>
#include <nlohmann/json.hpp>
#include <boost/uuid.hpp>
#include <nlohmann/json-schema.hpp>

#include "../file/json/Json.hpp"
#include "../debug/debug.hpp"
#include "parser/schemaTypes.hpp"
#include "pack.hpp"
#include "PackStatus.hpp"

namespace PackStatus {
	typedef uint16_t u16;
	typedef uint32_t u32;
	using std::vector, std::unordered_map, std::string, std::ofstream, Json::getJSON, Json::deduceFormatVersion, boost::uuids::string_generator, std::exception;
	using namespace std::filesystem;
	using namespace nlohmann;

	unordered_map<Pack::PackIdentifier, u16> statusCache;

	void refreshStatus(const char* rootDir) {
		//`packs` folder is gone.
		if (!exists(rootDir) || !is_directory(rootDir)) {
			create_directory(rootDir);
			return;
		}
		lout << "[Pack] Loading pack statuses!" << endl;
		//No diff here as well
		statusCache.clear();
		string temp(rootDir);
		temp += "\\config.json";
		if (!exists(temp) || !is_regular_file(temp)) {
			Json::saveJSON(temp);
			return;
		}
		auto _status = getJSON(temp.c_str());
		if (!_status) return;
		const json status = _status.value(), schemaJSON = json::parse(configSchema_raw);
		json_schema::json_validator validator(nullptr, json_schema::default_string_format_check);
		validator.set_root_schema(schemaJSON);
		try { const auto default_patch = validator.validate(status); }
		//We currently empty `config.json` directly if any error happened during validation.
		catch (exception e) {
			lerr << "[PackStatus] config.json is invalid, emptying it: " << e.what() << endl;
			Json::saveJSON(temp);
			return;
		}
		const u32 formatVersion = deduceFormatVersion(status, temp.c_str());
		if (formatVersion < LATEST_FORMAT_VERSION_INTERNAL) {
			//todo: Upgrade config file if LFVI is no longer 1 (which may take 1 year toevolve :) ).
		}
		else {
			auto data = status["data"].get<vector<json>>();
			for (u32 i = 0; i < data.size(); i++) {
				string_generator gen;
				Pack::PackIdentifier id(gen(data[i]["uuid"].get<string>()), data[i]["version"].get<u32>());
				u16 statusFlag = 0u;
				if (data[i]["valid"].get<bool>()) statusFlag |= Pack::PACK_STATUS_VALID;
				if (data[i]["enabled"].get<bool>()) statusFlag |= Pack::PACK_STATUS_ENABLED;
				if (data[i]["globalEnabled"].get<bool>()) statusFlag |=Pack::PACK_STATUS_GLOBAL_ENABLED;
				statusCache.emplace(id, statusFlag);
			}
		}
	}

	u16 getStatusFlags(const Pack::PackIdentifier& id) {
		auto p = statusCache.find(id);
		if (p == statusCache.end()) {
			//todo: `0u` is ambiguous. Throw error instead?
			return 0;
		}
		else return p->second;
	}

	void saveStatus(const PackStatusFlag& raw) {
		//todo
	}
}