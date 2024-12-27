#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>

#include "pack.hpp"
#include "../debug/Logger.hpp"
#include "js/V8Wrapper.hpp"
#include "json/JsonParser.hpp"
#include "json/jsontypes.hpp"
#include "PackManager.hpp"

typedef uint8_t u8;
typedef uint16_t u16;

using Logger::lout, std::string, std::vector, std::optional, std::filesystem::current_path, std::filesystem::directory_iterator, std::filesystem::exists, std::filesystem::is_directory, std::filesystem::is_regular_file, std::filesystem::path, std::filesystem::create_directory, std::move, nlohmann::json, JsonParser::getPatchedJSON;

namespace PackManager {
	vector<PackDesc> registry;

	void init() {
		V8Wrapper::init(current_path().string().c_str());
		JsonParser::init();
		refreshPacks();
	}

	void shutdown() {
		V8Wrapper::shutdown();
	}

	static u32 getFeatureFlags(const path& dirPath) {
		u32 featureFlags = 0u;
		for (const auto& directory : directory_iterator(dirPath)) {
			if (is_directory(directory)) {
				string name = directory.path().filename().string();
				if (name == "component") featureFlags |= PACK_FEATURE_COMPONENT;
				else if (name == "block") featureFlags |= PACK_FEATURE_BLOCK;
				else if (name == "fluid") featureFlags |= PACK_FEATURE_FLUID;
				else if (name == "entity") featureFlags |= PACK_FEATURE_ENTITY;
				else if (name == "item") featureFlags |= PACK_FEATURE_ITEM;
				else if (name == "worldgen") featureFlags |= PACK_FEATURE_WORLDGEN;
				else if (name == "biome") featureFlags |= PACK_FEATURE_BIOME;
				else if (name == "structure") featureFlags |= PACK_FEATURE_STRUCTURE;
				else if (name == "composite") featureFlags |= PACK_FEATURE_COMPOSITE;
				else if (name == "gui") featureFlags |= PACK_FEATURE_GUI;
				else if (name == "texture") featureFlags |= PACK_FEATURE_TEXTURE;
				else if (name == "sound") featureFlags |= PACK_FEATURE_SOUND;
				else if (name == "text") featureFlags |= PACK_FEATURE_TEXT;
				else if (name == "achievement") featureFlags |= PACK_FEATURE_ACHIEVEMENT;
				else if (name == "shader") featureFlags |= PACK_FEATURE_SHADER;
			}
		}
		return featureFlags;
	}

	static void getStatus(const char* rootDir) {

	}

	static void setStatus(PackIdentifier id) {

	}

	static u16 getStatusFlags(PackIdentifier id) {
		u16 result = 0u;
		//todo
		return result;
	}

	//Get packs.
	void refreshPacks(const char* rootDir) {
		//`packs` folder is gone.
		if (!exists(rootDir) || !is_directory(rootDir)) {
			create_directory(rootDir);
			return;
		}
		else {
			lout << "[PackManager] Loading packs!" << endl;
			//Probably not going to implement pack diff :)//We just reload every pack description for now.
			registry.clear();
			for (const auto& directory : directory_iterator(rootDir)) {
				path dirPathR = directory.path();
				lout << "[PackManager] found folder: " << dirPathR << endl;
				PackDesc result;
				string manifestPath = dirPathR.string() + "\\manifest.json";
				if (!exists(manifestPath) || !is_regular_file(manifestPath)) continue;
				optional<ManifestJSON> _manifest = getPatchedJSON<SchemaType::Manifest>(manifestPath.c_str());
				if (!_manifest) continue;
				ManifestJSON manifest = _manifest.value();
				result.dependencies = manifest.dependencies;
				//Generate feature flags. See `pack.hpp`.
				result.featureFlags = getFeatureFlags(dirPathR);
				result.metadata = manifest.metadata;
				result.minEngineVersion = manifest.minEngineVersion;
				result.packFlags = manifest.packFlags;
				result.packVersion = manifest.packVersion;
				result.uuid_f = manifest.uuid_f;
				result.statusFlags = getStatusFlags(PackIdentifier(result.uuid_f, result.packVersion));
				registry.emplace_back(move(result));
			}
			lout << "[PackManager] Found " << registry.size() << " pack" << (registry.size() == 1 ? "" : "s") << "." << endl;
		}
	}
}