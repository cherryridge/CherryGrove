#pragma once
#include <format>
#include <limits>
#include <string>
#include <vector>
#include <glaze/glaze.hpp>

#include "../meta.hpp"
#include "../util/json/helper.hpp"
#include "../util/json/wrappers/uuid.hpp"
#include "../util/lexical.hpp"
#include "base.hpp"

namespace Pack {
    typedef uint32_t u32;
    using std::string, std::vector, Util::Json::uuid_JSON;

    struct Manifest_v1 {
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
    };
}

namespace glz {
    typedef uint32_t u32;
    using std::format, std::numeric_limits, Pack::Manifest_v1;

    template <>
    struct meta<Manifest_v1> {
        using T = Manifest_v1;

        static constexpr auto modify = object(
            GLAZE_CONSTRAINT_DECL(id, !id.value.is_nil(),
                "UUID `00000000-0000-0000-0000-000000000000` is not a valid pack ID."
            ),
            GLAZE_CONSTRAINT_DECL(version, version > 0 && version < numeric_limits<u32>::max(),
                "`version` must be between 1 and u32::max - 1. Do not use these edge values. They're reserved for special purposes in pack version resolution."
            ),
            GLAZE_CONSTRAINT_DECL(minEngineVersion, minEngineVersion > 0 && minEngineVersion <= CG_ENGINE_VERSION,
                "`minEngineVersion` must be between 1 and the current engine version."
            ),
            GLAZE_CONSTRAINT_DECL(nameSpace, Util::isValidIdentifier(nameSpace),
                "`nameSpace` should be a valid conventional identifier."
            )
        );
    };
}