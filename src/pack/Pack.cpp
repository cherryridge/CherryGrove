#include <string>
#include <filesystem>
#include <vector>
#include <memory>
#include <optional>

#include "../debug/debug.hpp"
#include "../file/js/V8Wrapper.hpp"
#include "../file/json/Json.hpp"
#include "parser/schemaTypes.hpp"
#include "PackStatus.hpp"
#include "Pack.hpp"

namespace Pack {
	typedef uint8_t u8;
	typedef uint16_t u16;
	using std::string, std::vector, std::optional, std::filesystem::current_path, std::filesystem::directory_iterator, std::filesystem::exists, std::filesystem::is_directory, std::filesystem::is_regular_file, std::filesystem::path, std::filesystem::create_directory, std::move, Json::getPatchedJSON, PackStatus::PackStatusFlag;

	vector<PackDesc> registry;

	void init() {
		V8Wrapper::init(current_path().string().c_str());
		Json::init();
		refreshPacks();
	}

	void shutdown() {
		V8Wrapper::shutdown();
	}

	static u32 getFeatureFlags(const path& dirPath) {
		u32 featureFlags = 0u;
		for (const auto& directory : directory_iterator(dirPath)) if (is_directory(directory)) {
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
		return featureFlags;
	}

	//Store all packs' status to file.
	static void saveStatus() { for (u32 i = 0; i < registry.size(); i++) PackStatus::saveStatus(PackStatusFlag(registry[i].uuid_f, registry[i].packVersion, registry[i].statusFlags)); }

	//Get packs.
	void refreshPacks(const char* rootDir) {
		//`packs` folder is gone.
		if (!exists(rootDir) || !is_directory(rootDir)) {
			create_directory(rootDir);
			return;
		}
		PackStatus::refreshStatus(rootDir);
		lout << "[Pack] Loading packs!" << endl;
		//Probably not going to implement pack diff :)//We just reload every pack description for now.
		registry.clear();
		for (const auto& directory : directory_iterator(rootDir)) {
			auto& dirPathR = directory.path();
			if (is_regular_file(dirPathR)) continue;
			lout << "[Pack] found folder: " << dirPathR << endl;
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
			result.configFlags = manifest.configFlags;
			result.packVersion = manifest.packVersion;
			result.uuid_f = manifest.uuid_f;
			result.statusFlags = PackStatus::getStatusFlags(PackIdentifier(result.uuid_f, result.packVersion));
			registry.emplace_back(std::move(result));
		}
		lout << "[Pack] Found " << registry.size() << " pack" << (registry.size() == 1 ? "" : "s") << "." << endl;
	}
}
