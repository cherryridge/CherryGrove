#pragma once
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <boost/uuid.hpp>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

using std::vector, std::array, std::unique_ptr, std::string, boost::uuids::uuid;

//statusFlags map
constexpr u64 PACK_STATUS_VALID = 0x1u;
constexpr u64 PACK_STATUS_ENABLED = 0x2u;
constexpr u64 PACK_STATUS_GLOBAL_ENABLED = 0x4u;

//configFlags map
constexpr u64 PACK_CONFIG_WARN_ON_NS_DUPE = 0x1u;
constexpr u64 PACK_CONFIG_ALWAYS_LOAD = 0x2u;
constexpr u64 PACK_CONFIG_LICENSE_LIBRE = 0x4u;
constexpr u64 PACK_CONFIG_ALLOW_UNSAFE_EXECUTION = 0x8u;

//featureFlags map
constexpr u64 PACK_FEATURE_COMPONENT = 0x1u;
constexpr u64 PACK_FEATURE_BLOCK = 0x2u;
constexpr u64 PACK_FEATURE_FLUID = 0x4u;
constexpr u64 PACK_FEATURE_ENTITY = 0x8u;
constexpr u64 PACK_FEATURE_ITEM = 0x10u;
constexpr u64 PACK_FEATURE_WORLDGEN = 0x20u;
constexpr u64 PACK_FEATURE_BIOME = 0x40u;
constexpr u64 PACK_FEATURE_STRUCTURE = 0x80u;
constexpr u64 PACK_FEATURE_COMPOSITE = 0x100u;
constexpr u64 PACK_FEATURE_GUI = 0x200u;
constexpr u64 PACK_FEATURE_TEXTURE = 0x400u;
constexpr u64 PACK_FEATURE_SOUND = 0x800u;
constexpr u64 PACK_FEATURE_TEXT = 0x1000u;
constexpr u64 PACK_FEATURE_ACHIEVEMENT = 0x2000u;
constexpr u64 PACK_FEATURE_SHADER = 0x4000u;
//#define PACK_FEATURE_FONT 

struct PackIdentifier {
	//Yep, not saving space here as well ;-)
	uuid uuid_f;
	u32 packVersion;

	bool operator==(const PackIdentifier& other) const { return this->packVersion == other.packVersion && this->uuid_f == other.uuid_f; }
};

namespace std {
	template<>
	struct hash<PackIdentifier> {
		u64 operator()(const PackIdentifier& input) const {
			return hash<uuid>{}(input.uuid_f) ^ (hash<u32>{}(input.packVersion) << 1);
		}
	};
}

struct PackDesc {
	//`0`: nameSpace, `1`: name, `2`: description, `[2, size()-1]`: authors
	vector<string> metadata;
	uuid uuid_f;
	u32 packVersion;
	u32 minEngineVersion;
	u16 statusFlags;
	u16 configFlags;
	u32 featureFlags;
	vector<PackIdentifier> dependencies;
};