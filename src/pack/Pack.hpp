#pragma once
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <boost/uuid.hpp>

namespace Pack {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    using std::string, std::to_string, std::vector, std::array, std::unique_ptr, boost::uuids::uuid;

    //statusFlags map
    inline constexpr u64 PACK_STATUS_VALID = 0x1u;
    inline constexpr u64 PACK_STATUS_ENABLED = 0x2u;
    inline constexpr u64 PACK_STATUS_GLOBAL_ENABLED = 0x4u;

    //featureFlags map
    inline constexpr u64 PACK_FEATURE_COMPONENT = 0x1u;
    inline constexpr u64 PACK_FEATURE_BLOCK = 0x2u;
    inline constexpr u64 PACK_FEATURE_FLUID = 0x4u;
    inline constexpr u64 PACK_FEATURE_ENTITY = 0x8u;
    inline constexpr u64 PACK_FEATURE_ITEM = 0x10u;
    inline constexpr u64 PACK_FEATURE_WORLDGEN = 0x20u;
    inline constexpr u64 PACK_FEATURE_BIOME = 0x40u;
    inline constexpr u64 PACK_FEATURE_STRUCTURE = 0x80u;
    inline constexpr u64 PACK_FEATURE_COMPOSITE = 0x100u;
    inline constexpr u64 PACK_FEATURE_GUI = 0x200u;
    inline constexpr u64 PACK_FEATURE_TEXTURE = 0x400u;
    inline constexpr u64 PACK_FEATURE_SOUND = 0x800u;
    inline constexpr u64 PACK_FEATURE_TEXT = 0x1000u;
    inline constexpr u64 PACK_FEATURE_ACHIEVEMENT = 0x2000u;
    inline constexpr u64 PACK_FEATURE_SHADER = 0x4000u;
    //#define PACK_FEATURE_FONT 

    struct PackIdentifier {
        //Yep, not saving space here as well ;-)
        uuid uuid_f;
        u32 packVersion;

        PackIdentifier() = default;
        PackIdentifier(const uuid& _uuid, u32 version) : uuid_f(_uuid), packVersion(version) {}

        bool operator==(const PackIdentifier& other) const {
            return this->packVersion == other.packVersion && this->uuid_f == other.uuid_f;
        }
    };

    inline u64 hash_value(const PackIdentifier& input) noexcept {
        return std::hash<boost::uuids::uuid>{}(input.uuid_f) ^ (std::hash<uint32_t>{}(input.packVersion) << 1);
    }

    struct PackDesc {
        //`0`: nameSpace, `1`: name, `2`: description, `[2, size()-1]`: authors
        vector<string> metadata;
        uuid uuid_f;
        u32 packVersion, minEngineVersion, featureFlags;
        u16 statusFlags, configFlags;
        vector<PackIdentifier> dependencies;
    };

    void init();
    void shutdown();

    void refreshPacks(const char* rootDir = "packs");
};