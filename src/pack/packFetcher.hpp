#pragma once
#include <filesystem>
#include <format>
#include <string>
#include <utility>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>

#include "../debug/Logger.hpp"
#include "../umi/frontend/json/UmiJSON.hpp"
#include "../util/os/archiveReader.hpp"
#include "../util/os/filesystem.hpp"
#include "KnownPack.hpp"
#include "PackMetaInfo.hpp"
#include "registry.hpp"

namespace Pack {
    using std::move, std::string, std::vector, std::filesystem::path, std::filesystem::directory_iterator, std::filesystem::is_directory, std::filesystem::exists, std::to_underlying, std::format, boost::unordered_flat_map, Util::OS::u8, Util::OS::normalize, Util::OS::getU8String, Util::OS::ArchiveType, Util::OS::getArType, Util::OS::readFileFromAr, Util::OS::stripBOM, Util::Json::Latest, Util::Json::JSONKind::Manifest;

    //Try to add a pack to the registry.
    //If a pack with the same ID already exists, skip adding and log an error.
    //Currently `knownPacks` in settings is only used for version tracking (thus triggering further actions like migration preparation) and whether the user has explicitly disabled the pack. It have NOTHING to do with pack UUID collision handling!
    //todo: Currently we don't store the path in `knownPacks` in settings and we subsequently don't check if this time the pack loaded is from the same path as the last time. This means that if a pack is moved to a different pack root or from a pack root to a specific path (or vice versa), it will be treated as the same pack and the new path will be loaded without any information or warning whatsoever. We might want to add path tracking in `knownPacks` in the future to detect such cases and log a warning because I think this might be abusable for pack-level shadowing.
    inline void tryAddingPack(const PackMetaInfo& info) noexcept {
        const auto itRegistry = detail::registry.find(info.manifest.id);
        if (itRegistry != detail::registry.end()) {
            lerr << "[Pack] Pack with ID " << info.manifest.id.value() << " already exists in path " << itRegistry->second.pathStr << ", skipping adding the pack in path " << info.pathStr << ". Do not purposely clash pack IDs. If you want to overwrite a pack's behavior, use the same `nameSpace` and require yourself to load after the target pack." << endl;
            return;
        }
        detail::registry.emplace(info.manifest.id.value(), info);
        auto itKnownPacks = detail::knownPacks.find(info.manifest.id);
        if (itKnownPacks != detail::knownPacks.end()) {
            if (itKnownPacks->second.version != info.manifest.version) {
                lout << "[Pack] Pack " << info.manifest.id.value() << "'s version changed from " << itKnownPacks->second.version << " to " << info.manifest.version << ", todo: prepare for preparing migration." << endl;
            }
            itKnownPacks->second.version = info.manifest.version;
        }
        else detail::knownPacks.emplace(info.manifest.id.value(), KnownPack{
            .id = info.manifest.id,
            .version = info.manifest.version,
            .disabled = false
        });
        lout << "[Pack] Added pack: " << info.manifest.name << ", ID: " << info.manifest.id.value() << ", version: " << info.manifest.version << endl;
    }

    [[nodiscard]] inline bool parsePackManifest(const path& packPath, PackMetaInfo& result) noexcept {
        if (is_regular_file(packPath)) {
            const auto arType = getArType(packPath);
            if (arType == ArchiveType::NotASupportedArchive) {
                lerr << "[Pack] File " << packPath << " is not a supported archive type, skipping." << endl;
                return false;
            }
            else if (arType == ArchiveType::ExtensionDoesNotMatchContent) {
                lerr << "[Pack] File" << packPath << "has an extension that does not match its content type, skipping." << endl;
                return false;
            }
            vector<u8> manifestData;
            if (!readFileFromAr(packPath, "manifest.json", manifestData)) {
                lerr << "[Pack] Failed to read `manifest.json` from archive " << packPath << ", skipping." << endl;
                return false;
            }
            stripBOM(manifestData);
            Latest<Manifest> temp;
            if (!UmiJSON::readJSON<Manifest>(manifestData, temp)) {
                lerr << "[Pack] Failed to parse `manifest.json` in archive " << packPath << ", skipping." << endl;
                return false;
            }
            result = {
                .manifest = move(temp),
                .pathStr = getU8String(packPath),
                .disabled = false
            };
            return true;
        }
        else if (is_directory(packPath)) {
            const auto manifestPath = packPath / "manifest.json";
            if (!exists(manifestPath) || !is_regular_file(manifestPath)) {
                lout << "[Pack] No `manifest.json` found in " << packPath << ", skipping." << endl;
                return false;
            }
            Latest<Manifest> temp;
            if (!UmiJSON::readJSONFromFile<Manifest>(manifestPath, temp)) {
                lerr << "[Pack] Failed to parse manifest.json in " << packPath << ", skipping." << endl;
                return false;
            }
            result = {
                .manifest = move(temp),
                .pathStr = getU8String(packPath),
                .disabled = false
            };
            return true;
        }
        else {
            lout << "[Pack] Ignoring: " << packPath << endl;
            return false;
        }
    }

    inline void getPacksFromPackRoot(const string& rootStr) noexcept {
        path rootPath(rootStr);
        if (exists(rootPath) && is_directory(rootPath) && normalize(rootPath)) {
            lout << "[Pack] Traversing pack root: " << rootPath << endl;
            PackMetaInfo info;
            for(const auto& directoryEntry : directory_iterator(rootPath)) {
                if (parsePackManifest(directoryEntry.path(), info)) tryAddingPack(info);
                else lerr << "[Pack] Failed to parse pack: " << directoryEntry.path() << endl;
            }
        }
        else lerr << "[Pack] Ignoring invalid pack root: " << rootStr << endl;
    }
}