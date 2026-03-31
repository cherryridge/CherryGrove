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
    using std::span, std::vector, Util::Json::JSONKind, Util::Json::KindMeta, Util::FilePath, Util::OS::readFile;

    inline void init() noexcept {}
    inline void shutdown() noexcept {}

    template <JSONKind kind>
    [[nodiscard]] inline bool readJSON(const span<const u8> data, typename KindMeta<kind>::LatestType& result) noexcept {
        return KindMeta<kind>::process(data, result);
    }

    template <JSONKind kind, FilePath PathType>
    [[nodiscard]] inline bool readJSONFromFile(PathType&& path, typename KindMeta<kind>::LatestType& result, bool physfs) noexcept {
        vector<u8> data;
        if (!readFile(path, data, physfs)) {
            lerr << "Failed to read in JSON file: " << path << "\n";
            return false;
        }
        return readJSON<kind>(data, result);
    }
}