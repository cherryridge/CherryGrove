#pragma once
#include <string>
#include <boost/uuid.hpp>
#include <glaze/glaze.hpp>

#include "../pack/PackOptionDef.hpp"
#include "../util/json/helper.hpp"

namespace Pack {
    typedef int64_t i64;
    using std::string;

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
    using std::string, std::move, Pack::PackOptionValue;
    using enum Pack::PackOptionType;

    struct PackOptionValue_glz {
        string identifier, type;
        generic value;
    };

    template <>
    struct from<JSON, PackOptionValue> {
        template <auto Options>
        static void op(PackOptionValue& result, auto&& ctx, auto&& it, auto&& end) noexcept {
            PackOptionValue_glz temp;
            parse<JSON>::op<Options>(temp, ctx, it, end);

            result.identifier = move(temp.identifier);

            result.destroyUnion();
            if (temp.type == "boolean") {
                result.type = Boolean;
                GLAZE_CONSTRAINT_ASSERT(temp.value.is_boolean(), "Value for boolean option must be a boolean.")
                result.boolValue = temp.value.get<bool>();
            }
            else if (temp.type == "integer") {
                result.type = Integer;
                GLAZE_CONSTRAINT_ASSERT(temp.value.is_number(), "Value for integer option must be a number.")
                result.intValue = temp.value.as<i64>();
            }
            else if (temp.type == "float") {
                result.type = Float;
                GLAZE_CONSTRAINT_ASSERT(temp.value.is_number(), "Value for float option must be a number.")
                result.floatValue = temp.value.as<double>();
            }
            else if (temp.type == "string") {
                result.type = String;
                GLAZE_CONSTRAINT_ASSERT(temp.value.is_string(), "Value for string option must be a string.")
                new (&result.stringValue) string(temp.value.get<string>());
            }
            else if (temp.type == "enum") {
                result.type = Enum;
                GLAZE_CONSTRAINT_ASSERT(temp.value.is_string(), "Value for enum option must be a string.")
                new (&result.enumValue) string(temp.value.get<string>());
            }
            else GLAZE_CONSTRAINT_ASSERT(false, "Invalid option type.")
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
                    temp.value = input.boolValue;
                    break;
                case Integer:
                    temp.type = "integer";
                    temp.value = input.intValue;
                    break;
                case Float:
                    temp.type = "float";
                    temp.value = input.floatValue;
                    break;
                case String:
                    temp.type = "string";
                    temp.value = input.stringValue;
                    break;
                case Enum:
                    temp.type = "enum";
                    temp.value = input.enumValue;
                    break;
                case Count:
                    break;
            }

            serialize<JSON>::op<Options>(temp, ctx, b, ix);
        }
    };
}