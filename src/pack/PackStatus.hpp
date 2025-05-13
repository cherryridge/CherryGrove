#pragma once
#include <string>
#include <boost/uuid.hpp>

#include "parser/schemaTypes.hpp"
#include "Pack.hpp"

namespace PackStatus {
    typedef uint16_t u16;
    typedef uint32_t u32;

    using std::string, std::to_string, boost::uuids::uuid;

    struct PackStatusFlag {
        uuid uuid_f;
        u32 packVersion;
        u16 statusFlags;
        PackStatusFlag(const uuid& uuid_f, u32 packVersion, u16 statusFlags)
        : uuid_f(uuid_f), packVersion(packVersion), statusFlags(statusFlags) {}
    };

    void refreshStatus(const char* rootDir);
    u16 getStatusFlags(const Pack::PackIdentifier& id);
    void saveStatus(const PackStatusFlag& raw);

    inline const auto configSchema_raw = string(R"(
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "type": "object",
    "properties": {
        "formatVersion": {
            "type": "integer",
            "const": )") + to_string(LATEST_FORMAT_VERSION_INTERNAL) + string(R"(
        },
        "data": {
            "type": "array",
            "items": {
                "type": "object",
                "properties": {
                    "uuid": {
                        "type": "string",
                        "format": "uuid"
                    },
                    "version": {
                        "type": "integer",
                        "minimum": 1
                    },
                    "valid": {
                        "type": "boolean"
                    },
                    "enabled": {
                        "type": "boolean"
                    },
                    "globalEnabled": {
                        "type": "boolean"
                    }
                },
                "required": [
                    "uuid",
                    "version",
                    "valid",
                    "enabled",
                    "globalEnabled"
                ]
            }
        }
    },
    "required": [
        "formatVersion",
        "data"
    ]
})");
}
