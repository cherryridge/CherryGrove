#pragma once
#include <span>
#include <string>
#include <utility>
#include <glaze/glaze.hpp>

#include "../../debug/Logger.hpp"
#include "../../settings/JSONKind_Settings.hpp" // IWYU pragma: keep
#include "../../umi/frontend/json/UmiJSON.hpp" // IWYU pragma: keep
#include "../os/filesystem.hpp"
#include "formatVersion.hpp"
#include "JSONKind.hpp"

namespace Util::Json {
    typedef uint8_t u8;
    using std::span, std::string, std::move, glz::write_json_schema, Util::OS::writeFile, Util::OS::ExistBehavior, Logger::LOGGER_DYNAMIC_ERR, Logger::LOGGER_DYNAMIC_OUT;

    namespace detail {
        //This is not only for `u64` or number related schemas. The `_u64` actually means Glaze will store numeric values as `u64` internally, but it can be used for any schema. The default behavior for Glaze is to store numbers as `double`, which will cause precision loss for large integers.
        using SchemaJSON = glz::generic_u64;

        [[nodiscard]] inline bool isObjectSchemaNode(const SchemaJSON::object_t& object) noexcept {
            const auto typeIt = object.find("type");
            if (typeIt == object.end()) return false;

            const auto* type = typeIt->second.get_if<string>();
            return type != nullptr && *type == "object";
        }

        inline void moveRefSiblingsIntoAllOf(SchemaJSON& node) noexcept {
            auto* object = node.get_if<SchemaJSON::object_t>();
            if (object != nullptr) {
                for (auto& entry : *object) moveRefSiblingsIntoAllOf(entry.second);
                //Make every object's `additionalProperties` true to allow partial upgrades on user side.
                if (isObjectSchemaNode(*object)) object->insert_or_assign("additionalProperties", true);
                const auto refIt = object->find("$ref");
                if (refIt == object->end() || object->size() == 1) return;
                SchemaJSON refOnly;
                refOnly["$ref"] = move(refIt->second);
                object->erase(refIt);
                SchemaJSON::array_t allOf;
                allOf.emplace_back(move(refOnly));
                SchemaJSON allOfNode(allOf);
                object->insert_or_assign("allOf", move(allOfNode));
                return;
            }
            auto* array = node.get_if<SchemaJSON::array_t>();
            if (array == nullptr) return;
            for (auto& value : *array) moveRefSiblingsIntoAllOf(value);
        }

        [[nodiscard]] inline string makeRefSiblingsCompatible(const string& schema) noexcept {
            auto parsed = glz::read_json<SchemaJSON>(schema);
            if (!parsed.has_value()) return schema;
            moveRefSiblingsIntoAllOf(parsed.value());
            const auto result = parsed->dump();
            if (!result.has_value()) return schema;
            return result.value();
        }
    }

    #define GENERATE(kind) { \
        const auto expected = write_json_schema<Latest<JSONKind::kind>>(); \
        const string fileName = string(getJSONKindName(JSONKind::kind)) + ".schema.json"; \
        if (expected.has_value()) { \
            const string schema = detail::makeRefSiblingsCompatible(expected.value()); \
            if (!writeFile(fileName, span<const u8>(reinterpret_cast<const u8*>(schema.data()), schema.size()), ExistBehavior::Overwrite)) LOGGER_DYNAMIC_ERR("[Json] Failed to write ", fileName); \
            else LOGGER_DYNAMIC_OUT("[Json] Successfully generated JSON schema ", fileName); \
        } \
        else LOGGER_DYNAMIC_ERR("[Json] Failed to generate JSON schema ", fileName, ": ", expected.error()); \
    }

    inline void generateSchemas() noexcept {
        GENERATE(Manifest);
        GENERATE(Settings);
    }

    #undef GENERATE
}