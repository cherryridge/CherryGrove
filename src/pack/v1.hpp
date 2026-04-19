#pragma once
#include <limits>
#include <string>
#include <vector>
#include <glaze/glaze.hpp>

#include "../meta.hpp"
#include "../util/json/helpers.hpp"
#include "../util/lexical.hpp"
#include "../util/wrappers/uuid.hpp"
#include "PackVersionRange.hpp"

namespace Pack {
    typedef uint32_t u32;
    using std::string, std::vector, glz::schema, Util::Wrapper::uuid_JSON;

    JSON_STRUCT Manifest_v1 {
        u32 formatVersion{1};

        uuid_JSON id;
        u32 version{0};
        u32 minEngineVersion{0};
        string nameSpace;
        string name{"Unnamed Content Pack"};
        string description{"This content pack has no description."};
        vector<string> authors;
        vector<PackVersionRange> mustBefore;
        vector<PackVersionRange> mustExist;
        vector<PackVersionRange> sendWarning;
        vector<PackVersionRange> sendError;

        struct glaze_json_schema {
            schema id{
                .description = "Pack's UUID. Must be unique. Do not use full 0 UUID.",
            };
            schema version{
                .description = "Your pack's version, using `u32` integer. This field should be increment only. Do not use `0` or `u32::max`."
            };
            schema minEngineVersion{
                .description = "Your pack's minimum compatible engine version, inclusive."
            };
            schema nameSpace{
                .description = "Your pack's registry namespace. You may purposely set this to the same as other packs to attempt to overwrite them. This field should be a valid conventional identifier."
            };
            schema name{
                .description = "Human readable name for your pack."
            };
            schema description{
                .description = "Human readable description for your pack."
            };
            schema authors{
                .description = "Author credits for human reading.",
            };
            schema mustBefore{
                .description = "Pack + Pack version minimum & maximum that must be loaded before your pack loads."
            };
            schema mustExist{
                .description = "Pack + Pack version minimum & maximum that must be loaded alongside with your pack, but you claim no constraint on the order."
            };
            schema sendWarning{
                .description = "Pack + Pack version minimum & maximum that if loaded alongside with your pack, we will send user a warning about it on your behalf, which user can decide whether to stop loading or continue anyway. There is currently no way to customize the message."
            };
            schema sendError{
                .description = "Pack + Pack version minimum & maximum that if loaded alongside with your pack, we will stop pack loading and give user an error about it. There is currently no way to customize the message."
            };
        };
    };
}

GLAZE_STATIC_CONSTRAINT_BEGIN(Pack::Manifest_v1)
    GLAZE_STATIC_CONSTRAINT(id, !id.value().is_nil(),
        "UUID `00000000-0000-0000-0000-000000000000` is not a valid pack ID."
    ),
    GLAZE_STATIC_CONSTRAINT(version, version > 0 && version < std::numeric_limits<uint32_t>::max(),
        "`version` must be between 1 and u32::max - 1. Do not use these edge values. They're reserved for special purposes in pack version resolution."
    ),
    GLAZE_STATIC_CONSTRAINT(minEngineVersion, minEngineVersion > 0 && minEngineVersion <= CG_ENGINE_VERSION,
        "`minEngineVersion` must be between 1 and the current engine version."
    ),
    GLAZE_STATIC_CONSTRAINT(nameSpace, Util::isValidIdentifier(nameSpace),
        "`nameSpace` should be a valid conventional identifier."
    )
GLAZE_STATIC_CONSTRAINT_END