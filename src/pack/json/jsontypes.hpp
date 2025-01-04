#pragma once
#include <vector>
#include <string>
#include <tuple>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <boost/uuid.hpp>

#include "../pack.hpp"

typedef uint16_t u16;
typedef uint32_t u32;

using std::vector, std::string, boost::uuids::uuid, std::tuple, std::unique_ptr, std::unordered_map, std::function, nlohmann::json;

#define BASE_FV u32 formatVersion;

constexpr u32 LATEST_FORMAT_VERSION = 1;
constexpr u32 LATEST_FORMAT_VERSION_INTERNAL = 1;

/* Adding a Schema
 * 1. Add enum entry to `SchemaType`.
 * 2. Add `SchemaReturn` code instance.
 * 3. Add actual struct.
 * 4. Add corresponding `ProcessorPtr` to `ProcessorReg`.
 * 5. Add a folder named your schema in `json`, and do as the manifest folder.
 */

enum SchemaType {
	Manifest,
	//Component,
	//Block,
	//Geometry,
	//Entity,
	//Item,
	//Structure,
	//todo
	Count
};

//Actual types.
//The newest internal versions. We *should* keep formatVersion away from anywhere beyond the process functions and use them as a bridge.
struct ManifestJSON {
	//`0`: nameSpace, `1`: name, `2`: description, `[2, size()-1]`: authors
	vector<string> metadata;
	uuid uuid_f;
	u32 packVersion;
	u32 minEngineVersion;
	u16 configFlags;
	vector<PackIdentifier> dependencies;
};

//Deduce return type.
template <SchemaType T>
struct SchemaReturn;
template <>
struct SchemaReturn<SchemaType::Manifest> { using type = ManifestJSON; };

//Processor registry types.
template <typename T>
using ProcessorMap = unordered_map<u32, function<T(const json& input)>>;

template <typename T>
using ProcessorPtr = unique_ptr<ProcessorMap<T>>;

using ProcessorReg = tuple<
	ProcessorPtr<ManifestJSON>

>;