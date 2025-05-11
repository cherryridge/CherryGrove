#pragma once
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <optional>
#include <tuple>
#include <string>
#include <memory>
#include <filesystem>
#include <exception>

#include "../../pack/parser/manifest/ManifestProcs.hpp"
#include "../../debug/debug.hpp"

namespace Json {
    typedef uint8_t u8;
    typedef uint32_t u32;
    using nlohmann::json, std::optional, std::nullopt, std::get, std::tuple, nlohmann::json_schema::json_validator, nlohmann::json_schema::default_string_format_check, std::string, std::to_string, std::unique_ptr, std::get, std::exception, std::filesystem::path;

    extern ProcessorReg processorRegistry;

    //Initialize JSON processors.
    void init();

    string getSchemaFilePath(SchemaType type, const json& input, const char* filePath);
    optional<json> getJSON(const char* filePath);
    //filePath is purely for printing warnings.
    u32 deduceFormatVersion(const json& input, const char* filePath = "");

    inline const json emptyJson = json({});
    void saveJSON(const path& filePath, const json& output = emptyJson);

    template <SchemaType T>
    optional<typename SchemaReturn<T>::type> getPatchedJSON(const char* filePath) {
        const auto _input = getJSON(filePath);
        if (!_input) return nullopt;
        const json result = _input.value();
        const string schemaFilePath = getSchemaFilePath(T, result, filePath);
        auto _schemaJSON = getJSON(schemaFilePath.c_str());
        if (!_schemaJSON) {
            lerr << "[JSON] Schema not found: " << schemaFilePath << " for " << filePath << endl;
            return nullopt;
        }
        const json schemaJSON = _schemaJSON.value();
        json_validator validator(nullptr, default_string_format_check);
        validator.set_root_schema(schemaJSON);
        json patchedResult;
        try {
            const auto default_patch = validator.validate(result);
            patchedResult = result.patch(default_patch);
        }
        catch (exception& e) {
            lerr << "[JSON] Error while validating and patching " << filePath << ": " << e.what() << endl;
            return nullopt;
        }
        const u32 formatVersion = deduceFormatVersion(patchedResult, filePath);
        auto processors = get<T>(processorRegistry).get();
        auto p = processors->find(formatVersion);
        if (p == processors->end()) {
            lerr << "[JSON] `formatVersion`: " << formatVersion << " is invalid/deprecated! LatestformatVersion is " << LATEST_FORMAT_VERSION << endl;
            return nullopt;
        }
        return p->second(patchedResult);
    }
}