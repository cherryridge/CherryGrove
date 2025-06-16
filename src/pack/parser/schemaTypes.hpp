#pragma once
#include <vector>
#include <string>
#include <tuple>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/uuid.hpp>

#include "../Pack.hpp"

#define BASE_FV uint32_t formatVersion;

inline constexpr uint32_t LATEST_FORMAT_VERSION = 1;
inline constexpr uint32_t LATEST_FORMAT_VERSION_INTERNAL = 1;

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
    std::vector<std::string> metadata;
    boost::uuids::uuid uuid_f;
    uint32_t packVersion;
    uint32_t minEngineVersion;
    uint16_t configFlags;
    std::vector<Pack::PackIdentifier> dependencies;
};

//Deduce return type.
template <SchemaType T>
struct SchemaReturn;
template <>
struct SchemaReturn<SchemaType::Manifest> { using type = ManifestJSON; };

//Processor registry types.
template <typename T>
using ProcessorMap = boost::unordered_flat_map<uint32_t, std::function<T(const nlohmann::json& input)>>;

template <typename T>
using ProcessorPtr = std::unique_ptr<ProcessorMap<T>>;

using ProcessorReg = std::tuple<
    ProcessorPtr<ManifestJSON>

>;