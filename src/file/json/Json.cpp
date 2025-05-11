#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <filesystem>
#include <optional>
#include <memory>

#include "../../debug/debug.hpp"

#include "../../pack/parser/manifest/ManifestProcs.hpp"
#include "../../pack/parser/schemaTypes.hpp"

#include "Json.hpp"

namespace Json {
	typedef uint32_t u32;
	using std::optional, std::nullopt, nlohmann::json, std::string, std::filesystem::exists, std::filesystem::is_regular_file, std::filesystem::path, std::ofstream, std::ifstream, std::istreambuf_iterator, std::move;

	ProcessorReg processorRegistry;

	void init() {
		ProcessorPtr<ManifestJSON> manifest = ManifestProcs::getProcessors();
		get<SchemaType::Manifest>(processorRegistry) = std::move(manifest);
	}

	string getSchemaFilePath(SchemaType type, const json& input, const char* filePath) {
		const u32 formatVersion = deduceFormatVersion(input, filePath);
		string schemaFilePath = "assets/schemas/";
		switch (type) {
			case SchemaType::Manifest: schemaFilePath += "manifest/manifest_"; break;
			//case SchemaType::Component: schemaFilePath += "component/component_"; break;
			//case SchemaType::Block: schemaFilePath += "block/block_"; break;
			//case SchemaType::Geometry: schemaFilePath += "geometry/geometry_"; break;
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
		if (!file.is_open()) {
			lerr << "[JSON] Can't open file: " << filePath << endl;
			return nullopt;
		}
		const string temp((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
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
		if (!input.contains("formatVersion")) {
			lout << "[JSON] Warning: " << filePath << " no formatVersion found, using default value " << LATEST_FORMAT_VERSION << endl;
			return LATEST_FORMAT_VERSION;
		}
		else if (input["formatVersion"].type() != json::value_t::number_unsigned) {
			lout << "[JSON] Warning: " << filePath << " incorrect type of formatVersion found, using default value " << LATEST_FORMAT_VERSION << endl;
			return LATEST_FORMAT_VERSION;
		}
		else return input["formatVersion"].get<u32>();
	}

	void saveJSON(const path& filePath, const json& output) {
		if (!exists(filePath) || !is_regular_file(filePath)) { lout << "[JsonSaver] Warning: File not found, creating new file: " << filePath << endl; }
		ofstream stream(filePath);
		if (!stream.is_open()) {
			lerr << "[JsonSaver] File can't be opened: " << filePath << endl;
			return;
		}
		stream << output.dump(4);
		stream.close();
	}
}
