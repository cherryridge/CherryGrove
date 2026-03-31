#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
#include <physfs.h>

#include "../debug/Logger.hpp"
#include "../umi/frontend/json/JSON.hpp"
#include "../util/os/filesystem.hpp"

namespace Pack {
    typedef uint64_t u64;
    using std::string, std::vector, std::filesystem::path, std::filesystem::directory_iterator, std::filesystem::is_directory, std::filesystem::exists, std::pair, Util::OS::normalize, boost::unordered_flat_map, Util::Json::Latest, Util::Json::JSONKind::Manifest;

    namespace detail {
        inline constexpr const char* PACK_MOUNT_PHYSFS_ROOT = "packs/";
        inline constexpr const char* PACK_TEMP_PHYSFS_ROOT = "temp/";
    }
    
    inline bool getSinglePack(const path& packPath, Latest<Manifest>& result) noexcept {
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
            if (!UmiJSON::readJSONFromFile<Manifest>(manifestStr, result, true)) {
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
            if (!UmiJSON::readJSONFromFile<Manifest>(packPath / "manifest.json", result, false)) {
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

    inline void getPacksFromPackRoots(const vector<string>& potentialPackRoots, unordered_flat_map<uuid, PackDescWithPath>& registry) noexcept {
        for(u64 i = 0; i < potentialPackRoots.size(); i++) {
            path temp(potentialPackRoots[i]);
            if (exists(temp) && is_directory(temp) && normalize(temp)) {
                lout << "[Pack] Traversing pack root: " << potentialPackRoots[i] << endl;
                for(const auto& directoryEntry : directory_iterator(temp)) {
                    const auto& packPath = directoryEntry.path();
                    PackDesc2 desc;
                    if (getSinglePack(packPath, desc)) registry.emplace(desc._uuid, PackDescWithPath{
                        .desc = move(desc),
                        ._path = packPath
                    });
                }
            }
            else lerr << "[Pack] Ignoring invalid pack root: " << potentialPackRoots[i] << endl;
        }
    }
}