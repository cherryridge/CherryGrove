#pragma once
#include <cstdint>
#include <glaze/glaze.hpp>

#include "../util/json/helpers.hpp"
#include "../util/wrappers/uuid.hpp"

namespace Pack {
    typedef uint32_t u32;
    using glz::schema, Util::Wrapper::uuid_JSON;

    JSON_STRUCT KnownPack {
        uuid_JSON id;
        u32 version;
        bool disabled;

        struct glaze_json_schema {
            schema id{
                .description = "Pack UUID."
            };
            schema version{
                .description = "Known pack version.",
                .minimum = 1
            };
            schema disabled{
                .description = "Whether this pack is disabled.",
                .defaultValue = false
            };
        };
    };
}

GLAZE_STATIC_CONSTRAINT_BEGIN(Pack::KnownPack)
    GLAZE_STATIC_CONSTRAINT(id, !id.value().is_nil(),
        "UUID `00000000-0000-0000-0000-000000000000` is not a valid pack ID."
    ),
    GLAZE_STATIC_CONSTRAINT(version, version >= 1,
        "`0` is reserved for pack version."
    )
GLAZE_STATIC_CONSTRAINT_END