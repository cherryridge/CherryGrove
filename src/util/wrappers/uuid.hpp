#pragma once
#include <string>
#include <boost/container_hash/hash.hpp>
#include <boost/uuid.hpp>
#include <glaze/glaze.hpp>

#include "../implHashFor.hpp"
#include "../json/helpers.hpp"

namespace Util::Wrapper {
    using std::string, boost::uuids::uuid, boost::uuids::string_generator, boost::uuids::to_string;

    namespace detail {
        inline string_generator generator;
    }

    JSON_STRUCT uuid_JSON {
    private:
        uuid value{};
    public:
        uuid_JSON() noexcept = default;
        uuid_JSON(const uuid& value) noexcept : value(value) {}
        auto operator<=>(const uuid_JSON&) const noexcept = default;
        auto getValue() const noexcept { return value; }
    };

    [[nodiscard]] inline bool uuidFromString(const string& input, uuid_JSON& result) noexcept {
        try {
            result = {detail::generator(input)};
            return true;
        }
        catch (...) {
            return false;
        }
    }

    [[nodiscard]] inline string uuidToString(const uuid_JSON& input) noexcept { return to_string(input.getValue()); }
}

namespace glz {
    using std::string, boost::uuids::uuid, Util::Wrapper::uuid_JSON, Util::Wrapper::uuidFromString, Util::Wrapper::uuidToString;

    GLAZE_DYNAMIC_FROM_START(uuid_JSON)
        string temp;
        parse<JSON>::op<Options>(temp, ctx, it, end);
        GLAZE_DYNAMIC_CONSTRAINT(uuidFromString(temp, result), "`id` is not a valid UUID.")
    GLAZE_DYNAMIC_FROM_END

    GLAZE_DYNAMIC_TO_START(uuid_JSON)
        const string temp = uuidToString(input);
        serialize<JSON>::op<Options>(temp, ctx, b, ix);
    GLAZE_DYNAMIC_TO_END
}

IMPL_HASH_FOR(Util::Wrapper, uuid_JSON, 666555444,
    boost::hash_combine(seed, input.getValue());
)