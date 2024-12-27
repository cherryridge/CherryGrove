#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <optional>
#include <memory>

#include "../../debug/Logger.hpp"
#include "manifest/ManifestProcs.hpp"
#include "jsontypes.hpp"
#include "JsonParser.hpp"

typedef uint32_t u32;

using Logger::lout, Logger::lerr, std::optional, std::nullopt, std::endl, nlohmann::json, std::string, std::filesystem::exists, std::ifstream, std::istreambuf_iterator, std::move;

namespace JsonParser {
	ProcessorReg processorRegistry;

	void init() {
		ProcessorPtr<ManifestJSON> manifest = ManifestProcs::getProcessors();
		get<SchemaType::Manifest>(processorRegistry) = move(manifest);
	}

	string getSchemaFilePath(SchemaType type, const json& input, const char* filePath) {
		u32 formatVersion = deduceFormatVersion(input, filePath);
		string schemaFilePath = "assets/schemas/";
		switch (type) {
			case Manifest: schemaFilePath += "manifest/manifest_"; break;
			case Component: schemaFilePath += "component/component_"; break;
			case Block: schemaFilePath += "block/block_"; break;
			case Geometry: schemaFilePath += "geometry/geometry_"; break;
			default: return "";
		}
		schemaFilePath += to_string(formatVersion);
		schemaFilePath += ".json";
		return schemaFilePath;
	}

	optional<json> getJSON(const char* filePath) {
		if (!exists(filePath)) {
			lerr << "[JSON] File doesn't exist: " << filePath << endl;
			return nullopt;
		}
		ifstream file(filePath);
		if (!file) {
			lerr << "[JSON] Can't open file: " << filePath << endl;
			return nullopt;
		}
		string temp((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
		if (!temp.size()) {
			lerr << "[JSON] Can't read in file: " << filePath << endl;
			return nullopt;
		}
		try { return json::parse(temp, nullptr, true, true); }
		//Converts error.
		catch (...) {
			lerr << "[JSON] File malformed: " << filePath << endl;
			return nullopt;
		}
	}

	u32 deduceFormatVersion(const json& input, const char* filePath) {
		//We need formatVersion to do validation, so we first validate it manually.
		if (!input.contains(FV)) {
			lout << "[JSON] Warning: " << filePath << " no formatVersion found, using default value " << latestFormatVersion << endl;
			return latestFormatVersion;
		}
		else if (input[FV].type() != json::value_t::number_unsigned) {
			lout << "[JSON] Warning: " << filePath << " incorrect type of formatVersion found, using default value " << latestFormatVersion << endl;
			return latestFormatVersion;
		}
		else return input[FV].get<u32>();
	}
}