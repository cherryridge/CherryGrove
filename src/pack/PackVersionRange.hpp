#pragma once
#include <glaze/glaze.hpp>
#include <limits>

#include "../util/json/helpers.hpp"
#include "../util/wrappers/uuid.hpp"

namespace Pack {
    typedef uint32_t u32;
    using glz::schema, Util::Wrapper::uuid_JSON;

    //[min, max], both inclusive. If `max` is `0`, it means there is no upper bound. Use `min == 1` for the same intent.
    JSON_STRUCT PackVersionRange {
        uuid_JSON id;
        u32 min, max;

        struct glaze_json_schema {
            schema id{
                .description = "Pack UUID"
            };
            schema min{
                .description = "Minimum applicable version, inclusive. Use `0` for unlimited backwards."
            };
            schema max{
                .description = "Maximum applicable version, inclusive. Use `0` for unlimited forwards. Do not use `u32::max`."
            };
        };

        [[nodiscard]] bool operator==(const PackVersionRange& other) const noexcept {
            return id == other.id && min == other.min && max == other.max;
        }

        [[nodiscard]] bool contains(const uuid_JSON& packId, u32 packVersion) const noexcept {
            if (id != packId) return false;
            if (max == 0) return packVersion >= min;
            else return packVersion >= min && packVersion <= max;
        }
    };
}

GLAZE_STATIC_CONSTRAINT_BEGIN(Pack::PackVersionRange)
    GLAZE_STATIC_CONSTRAINT(id, !id.value().is_nil(),
        "UUID `00000000-0000-0000-0000-000000000000` is not a valid pack ID."
    ),
    GLAZE_STATIC_CONSTRAINT(max, max != std::numeric_limits<uint32_t>::max(),
        "Do not use `u32::max`. Use `0` instead."
    )
GLAZE_STATIC_CONSTRAINT_END