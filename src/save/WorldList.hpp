#pragma once
#include <ctime>
#include <filesystem>
#include <string>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>

#include <nbt/nbt.hpp>

#include "../debug/Logger.hpp"
#include "../util/os/filesystem.hpp"
#include "saveBase.hpp"

namespace Save {
    using std::time_t, std::string, std::vector, std::filesystem::exists, std::filesystem::is_directory, std::filesystem::create_directory, std::filesystem::directory_iterator, std::filesystem::path, boost::unordered_flat_map, Util::OS::getU8String;

    struct WorldInfo {
        string folderName, name;
        time_t createdTime, lastModifiedTime;
        u32 engineVersion;
    };

    namespace detail {
        inline vector<WorldInfo> worldList;
    }

    inline void refreshWorldList(const char* rootDir) noexcept {
        if (!exists(rootDir) || !is_directory(rootDir)) {
            create_directory(rootDir);
            return;
        }
        lout << "[WorldList] Loading saves!" << endl;
        for (const auto& directory : directory_iterator(rootDir)) {
            const auto& dirPath = directory.path();
            if (is_regular_file(dirPath)) continue;
            lout << "[WorldList] found directory: " << dirPath << endl;
            const path metaPath = dirPath / "world.cgb";
            if (!exists(metaPath) || !is_regular_file(metaPath)) continue;
            unordered_flat_map<string, NBT::Tag> result;
            if (!NBT::read(getU8String(metaPath).c_str(), result)) {
                lout << "[WorldList] failed to read world.cgb in " << dirPath << endl;
                continue;
            }
            //todo: Resolve `world.cgb` structure properly
        }
    }
}