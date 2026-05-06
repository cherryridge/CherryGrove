#pragma once
#include <ctime>
#include <filesystem>
#include <string>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
#include <nbt/nbt.hpp>

#include "../../util/nbt/latest.hpp"

namespace Save {
    typedef uint8_t u8;
    typedef uint32_t u32;
    using std::filesystem::path, std::string, std::vector;

    struct WorldInfo_v1 {
        path folderPath;
        string name;
        time_t createdTime, lastModified;
        u32 engineVersion;
        vector<u8> thumbnail;
    };
}

REGISTER_LATEST_NBT(WorldInfo, Save::WorldInfo_v1);

namespace Save {
    typedef uint32_t u32;
    using boost::unordered_flat_map, NBT::memberOr, NBT::Types, Util::NBT::Latest_NBT, Util::NBT::NBTKind::WorldInfo;

    [[nodiscard]] inline Latest_NBT<WorldInfo> parse_v1(const unordered_flat_map<string, NBT::Tag>& data) noexcept {
        return {
            .name = memberOr<Types::String>(data, "name", "Unnamed World"),
            .createdTime = memberOr<Types::IVarInt>(data, "createdTime", 0),
            .lastModified = memberOr<Types::IVarInt>(data, "lastModifiedTime", 0),
            .engineVersion = static_cast<u32>(memberOr<Types::UVarInt>(data, "engineVersion", 0)),
            .thumbnail = memberOr<Types::ArrayRaw>(data, "thumbnail", {})
        };
    }
}