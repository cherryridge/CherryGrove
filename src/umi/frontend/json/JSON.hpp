#pragma once
#include <span>
#include <vector>

#include "../../../debug/Logger.hpp"
#include "../../../util/concepts.hpp"
#include "../../../util/json/formatVersion.hpp"
#include "../../../util/os/filesystem.hpp"

#include "manifest/main.hpp"

namespace UmiJSON {
    typedef uint8_t u8;
    using std::span, std::vector, Util::Json::JSONKind, Util::Json::KindMeta, Util::FilePath, Util::OS::readFile, Util::OS::writeFile, Util::OS::ExistBehavior;

    inline void init() noexcept {}
    inline void shutdown() noexcept {}

    template <JSONKind kind>
    [[nodiscard]] inline bool readJSON(const span<const u8> data, typename KindMeta<kind>::LatestType& result) noexcept { return KindMeta<kind>::read(data, result); }

    template <JSONKind kind, bool physfs, FilePath PathType>
    [[nodiscard]] inline bool readJSONFromFile(PathType&& path, typename KindMeta<kind>::LatestType& result) noexcept {
        vector<u8> data;
        if (!readFile<physfs>(path, data)) {
            lerr << "Failed to read in JSON file: " << path << "\n";
            return false;
        }
        return readJSON<kind>(data, result);
    }

    template <JSONKind kind>
    [[nodiscard]] inline bool writeJSON(const typename KindMeta<kind>::LatestType& input, vector<u8>& result) noexcept { return KindMeta<kind>::write(input, result); }

    template <JSONKind kind, FilePath PathType>
    [[nodiscard]] inline bool writeJSONToFile(const typename KindMeta<kind>::LatestType& input, PathType&& path, ExistBehavior existingBehavior) noexcept {
        vector<u8> data;
        if (!writeJSON<kind>(input, data)) {
            lerr << "Failed to write JSON data for file: " << path << "\n";
            return false;
        }
        return writeFile(path, data, existingBehavior);
    }
}