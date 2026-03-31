#pragma once
#include <string>
#include <boost/uuid/uuid.hpp>
#include <glaze/glaze.hpp>
#include <utility>

#include "../pack/PackOptionDef.hpp"

namespace Settings {
    typedef int64_t i64;
    using std::string, Pack::PackOptionType;

    struct PackOptionValue {
        using enum PackOptionType;

        string identifier;
        union {
            bool boolValue;
            i64 intValue;
            double floatValue;
            string stringValue;
            string enumValue;
        };
        PackOptionType type;

        [[nodiscard]] explicit PackOptionValue() noexcept : type(Count) {}

        #define TYPE [[nodiscard]] PackOptionValue(const string& identifier,
        #define INIT ) noexcept : identifier(identifier),

        TYPE bool boolValue
        INIT boolValue(boolValue), type(Boolean) {}

        TYPE i64 intValue
        INIT intValue(intValue), type(Integer) {}

        TYPE double floatValue
        INIT floatValue(floatValue), type(Float) {}

        template <PackOptionType T> requires (T == String)
        TYPE const string& stringValue
        INIT stringValue(stringValue), type(String) {}

        template <PackOptionType T> requires (T == Enum)
        TYPE const string& enumValue
        INIT enumValue(enumValue), type(Enum) {}

        #undef TYPE
        #undef INIT

        void destroyUnion() noexcept {
            switch (type) {
                case Boolean:
                case Integer:
                case Float:
                    break;
                case String:
                    stringValue.~string();
                    break;
                case Enum:
                    enumValue.~string();
                    break;
                case Count:
                    break;
            }
        }

        PackOptionValue& operator=(const PackOptionValue& other) noexcept {
            identifier = other.identifier;

            destroyUnion();
            switch (other.type) {
                case Boolean:
                    boolValue = other.boolValue;
                    break;
                case Integer:
                    intValue = other.intValue;
                    break;
                case Float:
                    floatValue = other.floatValue;
                    break;
                case String:
                    new (&stringValue) string(other.stringValue);
                    break;
                case Enum:
                    new (&enumValue) string(other.enumValue);
                    break;
                case Count:
                    break;
            }
            type = other.type;

            return *this;
        }

        [[nodiscard]] PackOptionValue(const PackOptionValue& other) noexcept : type(Count) { operator=(other); }

        ~PackOptionValue() { destroyUnion(); }
    };
}

namespace glz {
    typedef int64_t i64;
    using std::string, std::move, Settings::PackOptionValue;
    using enum Pack::PackOptionType;

    struct PackOptionValue_glz {
        string identifier, type;
        //Yep, because `Enum` and `String` both use `string` as their value type, we have to parse manually.
        raw_json value;
    };

    template <>
    struct from<JSON, PackOptionValue> {
        template <auto Options>
        static void op(PackOptionValue& result, auto&& ctx, auto&& it, auto&& end) noexcept {
            PackOptionValue_glz temp;
            parse<JSON>::op<Options>(temp, ctx, it, end);

            result.identifier = move(temp.identifier);

            result.destroyUnion();
            const string& type = temp.type;
            const string& raw = temp.value.str;
            const auto rawIt = raw.data();
            const auto rawEnd = rawIt + raw.size();

            if (type == "boolean") {
                parse<JSON>::op<Options>(result.boolValue, ctx, rawIt, rawEnd);
                result.type = Boolean;
            }
            else if (type == "integer") {
                parse<JSON>::op<Options>(result.intValue, ctx, rawIt, rawEnd);
                result.type = Integer;
            }
            else if (type == "float") {
                parse<JSON>::op<Options>(result.floatValue, ctx, rawIt, rawEnd);
                result.type = Float;
            }
            else if (type == "string") {
                new (&result.stringValue) string();
                parse<JSON>::op<Options>(result.stringValue, ctx, rawIt, rawEnd);
                result.type = String;
            }
            else if (type == "enum") {
                new (&result.enumValue) string();
                parse<JSON>::op<Options>(result.enumValue, ctx, rawIt, rawEnd);
                result.type = Enum;
            }
        }
    };

    template <>
    struct to<JSON, PackOptionValue> {
        template <auto Options>
        static void op(const PackOptionValue& input, is_context auto&& ctx, auto& b, auto& ix) noexcept {
            PackOptionValue_glz temp;

            temp.identifier = input.identifier;

            switch (input.type) {
                case Boolean:
                    temp.type = "boolean";
                    write<Options>(input.boolValue, temp.value.str, ctx);
                    break;
                case Integer:
                    temp.type = "integer";
                    write<Options>(input.intValue, temp.value.str, ctx);
                    break;
                case Float:
                    temp.type = "float";
                    write<Options>(input.floatValue, temp.value.str, ctx);
                    break;
                case String:
                    temp.type = "string";
                    write<Options>(input.stringValue, temp.value.str, ctx);
                    break;
                case Enum:
                    temp.type = "enum";
                    write<Options>(input.enumValue, temp.value.str, ctx);
                    break;
                case Count:
                    break;
            }

            serialize<JSON>::op<Options>(temp, ctx, b, ix);
        }
    };
}