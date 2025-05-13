#include <memory>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <boost/uuid.hpp>

#include "ManifestProcs.hpp"
#include "../schemaTypes.hpp"

namespace ManifestProcs {
    typedef int32_t i32;
    typedef uint16_t u16;
    typedef uint32_t u32;

    using std::vector, std::string, nlohmann::json, boost::uuids::string_generator, std::make_unique, std::move;

    //2024.12.27 - CURRENT LFV: 1
    static vector<Pack::PackIdentifier> processDependenciesv1(const vector<json>& input) {
        vector<Pack::PackIdentifier> result;
        for (i32 i = 0; i < input.size(); i++) {
            Pack::PackIdentifier temp;
            string_generator gen;
            temp.uuid_f = gen(input[i]["uuid"].get<string>());
            temp.packVersion = input[i]["version"].get<u32>();
            result.push_back(temp);
        }
        return result;
    }
    static u16 processPackFlagsv1(const json& input) {
        u16 configFlags = 0;
        if (input["allowUnsafeExecution"].get<bool>()) configFlags |= Pack::PACK_CONFIG_ALLOW_UNSAFE_EXECUTION;
        if (input["alwaysLoad"].get<bool>()) configFlags |= Pack::PACK_CONFIG_ALWAYS_LOAD;
        if (input["licenseLibre"].get<bool>()) configFlags |= Pack::PACK_CONFIG_LICENSE_LIBRE;
        if (input["warnOnNSDupe"].get<bool>()) configFlags |= Pack::PACK_CONFIG_WARN_ON_NS_DUPE;
        return configFlags;
    }
    static ManifestJSON processv1(const json& input) {
        ManifestJSON result;
        string_generator gen;
        result.uuid_f = gen(input["uuid"].get<string>());
        result.packVersion = input["version"].get<u32>();
        result.minEngineVersion = input["minEngineVersion"].get<u32>();
        result.configFlags = processPackFlagsv1(input["configs"].get<json>());
        vector<string> metadata;
        metadata.push_back(input["nameSpace"].get<string>());
        metadata.push_back(input["name"].get<string>());
        metadata.push_back(input["description"].get<string>());
        auto _authors = input["authors"].get<vector<string>>();
        metadata.insert(metadata.end(), _authors.begin(), _authors.end());
        result.dependencies = processDependenciesv1(input["dependencies"].get<vector<json>>());
;        return result;
    }

    ProcessorPtr<ManifestJSON> getProcessors() {
        ProcessorMap<ManifestJSON> result;
        result.emplace(1, processv1);
        return make_unique<ProcessorMap<ManifestJSON>>(move(result));
    }
}
