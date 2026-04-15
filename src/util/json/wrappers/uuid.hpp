#pragma once
#include <string>
#include <boost/uuid.hpp>
#include <glaze/glaze.hpp>

#include "../../uuid.hpp"

namespace Util::Json {
    struct uuid_JSON {
        uuid value{};
        auto operator<=>(const uuid_JSON&) const noexcept = default;
    };
}

namespace glz {
    using std::string, boost::uuids::uuid, Util::Json::uuid_JSON;

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
            result.value = Util::uuidFromString(temp);
        }
    };

    template <>
    struct to<JSON, uuid_JSON> {
        template <auto Options>
        static void op(const uuid_JSON& input, is_context auto&& ctx, auto& b, auto& ix) noexcept {
            const string temp = Util::uuidToString(input.value);
            serialize<JSON>::op<Options>(temp, ctx, b, ix);
        }
    };
}
/*namespace glz {
    template <>
    struct meta<Util::Json::uuid_JSON> {
        using T = Util::Json::uuid_JSON;
        using mimic = std::string;

        static constexpr auto readUuid = [](T& self, const std::string& input) {
            self.value = Util::uuidFromString(input);
        };

        static constexpr auto writeUuid = [](const T& self) {
            return Util::uuidToString(self.value);
        };

        static constexpr auto value = glz::custom<readUuid, writeUuid>;
    };
}*/