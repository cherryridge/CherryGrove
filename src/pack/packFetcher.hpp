#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
#include <physfs.h>

#include "../debug/Logger.hpp"
#include "../umi/frontend/json/JSON.hpp"
#include "../util/os/filesystem.hpp"
#include "KnownPack.hpp"
#include "PackMetaInfo.hpp"
#include "registry.hpp"

namespace Pack {
    using std::move, std::string, std::vector, std::filesystem::path, std::filesystem::directory_iterator, std::filesystem::is_directory, std::filesystem::exists, Util::OS::normalize, boost::unordered_flat_map, Util::Json::Latest, Util::Json::JSONKind::Manifest;

    namespace detail {
        inline constexpr const char* PACK_MOUNT_PHYSFS_ROOT = "packs/";
        inline constexpr const char* PACK_TEMP_PHYSFS_ROOT = "temp/";
    }

    inline void tryAddingPack(const PackMetaInfo& info) noexcept {
        const auto itRegistry = detail::registry.find(info.manifest.id);
        if (itRegistry != detail::registry.end()) {
            lerr << "[Pack] Pack with ID " << info.manifest.id << " already exists in path " << itRegistry->second.path_ << ", skipping adding the pack in path " << info.path_ << ". Do not purposely clash pack IDs. If you want to overwrite a pack's behavior, use the same `nameSpace` and require yourself to load after the target pack." << endl;
            return;
        }
        detail::registry.emplace(info.manifest.id, info);
        auto itKnownPacks = detail::knownPacks.find(info.manifest.id);
        if (itKnownPacks != detail::knownPacks.end()) {
            if (itKnownPacks->second.version != info.manifest.version) {
                lout << "[Pack] Pack " << info.manifest.id << "'s version changed from " << itKnownPacks->second.version << " to " << info.manifest.version << ", todo: prepare for preparing migration." << endl;
            }
            itKnownPacks->second.version = info.manifest.version;
        }
        else detail::knownPacks.emplace(info.manifest.id, KnownPack{
            .id = info.manifest.id,
            .version = info.manifest.version,
            .disabled = false
        });
        lout << "[Pack] Added pack: " << info.manifest.name << ", ID: " << info.manifest.id << ", version: " << info.manifest.version << endl;
    }

    [[nodiscard]] inline bool parsePackManifest(const path& packPath, PackMetaInfo& result) noexcept {
        if (is_regular_file(packPath)) {
            const auto ext = packPath.extension().string(), pathStr = packPath.string();
            //Don't error on regular files. Just ignore them.
            if (ext != ".zip" && ext != ".cgp" && ext != ".7z") {
                lout << "[Pack] Ignoring non-archive file: " << packPath << endl;
                return false;
            }
            if (!PHYSFS_mount(pathStr.c_str(), detail::PACK_TEMP_PHYSFS_ROOT, false)) {
                lerr << "[Pack] Failed to mount archive: " << packPath << ", error: " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()) << endl;
                return false;
            }
            const auto manifestStr = string(detail::PACK_TEMP_PHYSFS_ROOT) + "/manifest.json";
            if (!PHYSFS_exists(manifestStr.c_str())) {
                lout << "[Pack] No manifest.json found in " << packPath << ", skipping." << endl;
                PHYSFS_unmount(detail::PACK_TEMP_PHYSFS_ROOT);
                return false;
            }
            Latest<Manifest> temp;
            if (!UmiJSON::readJSONFromFile<Manifest, true>(manifestStr, temp)) {
                lerr << "[Pack] Failed to parse manifest.json in " << packPath << ", skipping." << endl;
                PHYSFS_unmount(detail::PACK_TEMP_PHYSFS_ROOT);
                return false;
            }
            PHYSFS_unmount(detail::PACK_TEMP_PHYSFS_ROOT);
            return true;
        }
        else if (is_directory(packPath)) {
            const auto manifestPath = packPath / "manifest.json";
            if (!exists(manifestPath) || !is_regular_file(manifestPath)) {
                lout << "[Pack] No manifest.json found in " << packPath << ", skipping." << endl;
                return false;
            }
            Latest<Manifest> temp;
            if (!UmiJSON::readJSONFromFile<Manifest, false>(packPath / "manifest.json", temp)) {
                lerr << "[Pack] Failed to parse manifest.json in " << packPath << ", skipping." << endl;
                return false;
            }
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