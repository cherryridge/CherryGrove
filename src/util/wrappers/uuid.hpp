#pragma once
#include <string>
#include <boost/container_hash/hash.hpp>
#include <boost/uuid.hpp>
#include <glaze/glaze.hpp>

#include "../implHashFor.hpp"
#include "../json/helper.hpp"

namespace Util::Wrapper {
    using std::string, boost::uuids::uuid, boost::uuids::string_generator, boost::uuids::to_string;

    namespace detail {
        inline string_generator generator;
    }

    struct uuid_JSON {
        uuid value{};
        auto operator<=>(const uuid_JSON&) const noexcept = default;
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

    [[nodiscard]] inline string uuidToString(const uuid_JSON& input) noexcept { return to_string(input.value); }
}

namespace glz {
    using std::string, boost::uuids::uuid, Util::Wrapper::uuid_JSON, Util::Wrapper::uuidFromString, Util::Wrapper::uuidToString;

    template <>
    struct meta<uuid_JSON> {
        using mimic = string;
        static constexpr bool custom_read = true;
        static constexpr bool custom_write = true;
    };

    template <>
    struct from<JSON, uuid_JSON> {
        template <auto Options>
        static void op(uuid_JSON& result, auto&& ctx, auto&& it, auto&& end) noexcept {
            string temp;
            parse<JSON>::op<Options>(temp, ctx, it, end);
            GLAZE_CONSTRAINT_ASSERT(uuidFromString(temp, result), "`id` is not a valid UUID.")
        }
    };

    template <>
    struct to<JSON, uuid_JSON> {
        template <auto Options>
        static void op(const uuid_JSON& input, is_context auto&& ctx, auto& b, auto& ix) noexcept {
            const string temp = uuidToString(input);
            serialize<JSON>::op<Options>(temp, ctx, b, ix);
        }
    };
}

IMPL_HASH_FOR(Util::Wrapper, uuid_JSON, 666555444,
    boost::hash_combine(seed, input.value);
)