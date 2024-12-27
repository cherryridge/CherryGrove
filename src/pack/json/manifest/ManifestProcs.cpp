#include <nlohmann/json.hpp>
#include <memory>
#include <string>
#include <boost/uuid.hpp>

#include "ManifestProcs.hpp"
#include "../JsonParser.hpp"
#include "../jsontypes.hpp"

typedef int32_t i32;

using std::string, nlohmann::json, boost::uuids::string_generator, std::unique_ptr, std::make_unique, std::move;

namespace ManifestProcs {

	//2024.12.27 - CURRENT LFV: 1
	static vector<PackIdentifier> processDependenciesv1(const vector<json>& input) {
		vector<PackIdentifier> result;
		for (i32 i = 0; i < input.size(); i++) {
			PackIdentifier temp;
			string_generator gen;
			temp.uuid_f = gen(input[i]["uuid"].get<string>());
			temp.packVersion = input[i]["version"].get<u32>();
			result.push_back(temp);
		}
		return result;
	}
	static u16 processPackFlagsv1(const json& input) {
		u16 packFlags = 0u;
		if (input["allowUnsafeExecution"].get<bool>()) packFlags |= PACK_ALLOW_UNSAFE_EXECUTION;
		if (input["alwaysLoad"].get<bool>()) packFlags |= PACK_ALWAYS_LOAD;
		if (input["licenseLibre"].get<bool>()) packFlags |= PACK_LICENSE_LIBRE;
		if (input["warnOnNSDupe"].get<bool>()) packFlags |= PACK_WARN_ON_NS_DUPE;
		return packFlags;
	}
	static ManifestJSON processv1(const json& input) {
		ManifestJSON result;
		string_generator gen;
		result.uuid_f = gen(input["uuid"].get<string>());
		result.packVersion = input["version"].get<u32>();
		result.minEngineVersion = input["minEngineVersion"].get<u32>();
		result.packFlags = processPackFlagsv1(input["configs"].get<json>());
		vector<string> metadata;
		metadata.push_back(input["nameSpace"].get<string>());
		metadata.push_back(input["name"].get<string>());
		metadata.push_back(input["description"].get<string>());
		auto _authors = input["authors"].get<vector<string>>();
		metadata.insert(metadata.end(), _authors.begin(), _authors.end());
		result.dependencies = processDependenciesv1(input["dependencies"].get<vector<json>>());
;		return result;
	}

	ProcessorPtr<ManifestJSON> getProcessors() {
		ProcessorMap<ManifestJSON> result;
		result.emplace(1, processv1);
		return make_unique<ProcessorMap<ManifestJSON>>(move(result));
	}
}