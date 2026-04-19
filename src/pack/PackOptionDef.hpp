#pragma once
#include <string>
#include <vector>
#include <glaze/glaze.hpp>

#include "../util/json/helpers.hpp"

namespace Pack {
    typedef uint8_t u8;

    enum struct PackOptionType : u8 {
        Boolean, Integer, Float, String, Enum, Count
    };
}

GLAZE_ENUM_START(Pack::PackOptionType)
    GLAZE_ENUM("boolean", Boolean),
    GLAZE_ENUM("integer", Integer),
    GLAZE_ENUM("float", Float),
    GLAZE_ENUM("string", String),
    GLAZE_ENUM("enum", Enum)
GLAZE_ENUM_END

namespace Pack {
    typedef int64_t i64;
    using std::string, std::vector;

    JSON_STRUCT PackOptionDef {
        using enum PackOptionType;

        string identifier, label, description;
        union {
            bool defaultBool;
            struct {
                i64 defaultInt, intMin, intMax;
            };
            struct {
                double defaultFloat, floatMin, floatMax;
            };
            string defaultString;
            struct {
                string defaultEnum;
                vector<string> enumValues;
            };
        };
        PackOptionType type;

        [[nodiscard]] explicit PackOptionDef() noexcept : type(Count) {}

        #define TYPE [[nodiscard]] PackOptionDef(const string& identifier, const string& label, const string& description,
        #define INIT ) noexcept : identifier(identifier), label(label), description(description),

        TYPE bool defaultBool
        INIT defaultBool(defaultBool), type(Boolean) {}

        TYPE i64 defaultInt, i64 intMin, i64 intMax
        INIT defaultInt(defaultInt), intMin(intMin), intMax(intMax), type(Integer) {}
    
        TYPE double defaultFloat, double floatMin, double floatMax
        INIT defaultFloat(defaultFloat), floatMin(floatMin), floatMax(floatMax), type(Float) {}

        TYPE const string& defaultString
        INIT defaultString(defaultString), type(String) {}

        TYPE const string& defaultEnum, const vector<string>& enumValues
        INIT defaultEnum(defaultEnum), enumValues(enumValues), type(Enum) {}

        #undef TYPE
        #undef INIT

        void destroyUnion() noexcept {
            switch (type) {
                case Boolean:
                case Integer:
                case Float:
                    break;
                case String:
                    defaultString.~string();
                    break;
                case Enum:
                    defaultEnum.~string();
                    enumValues.~vector();
                    break;
                case Count:
                    break;
            }
        }

        PackOptionDef& operator=(const PackOptionDef& other) noexcept {
            identifier = other.identifier;
            label = other.label;
            description = other.description;

            destroyUnion();
            switch (other.type) {
                case Boolean:
                    defaultBool = other.defaultBool;
                    break;
                case Integer:
                    defaultInt = other.defaultInt;
                    intMin = other.intMin;
                    intMax = other.intMax;
                    break;
                case Float:
                    defaultFloat = other.defaultFloat;
                    floatMin = other.floatMin;
                    floatMax = other.floatMax;
                    break;
                case String:
                    new (&defaultString) string(other.defaultString);
                    break;
                case Enum:
                    new (&defaultEnum) string(other.defaultEnum);
                    new (&enumValues) vector<string>(other.enumValues);
                    break;
                case Count:
                    break;
            }
            type = other.type;

            return *this;
        }

        [[nodiscard]] PackOptionDef(const PackOptionDef& other) noexcept : type(Count) { operator=(other); }

        ~PackOptionDef() { destroyUnion(); }
    };
}

namespace glz {
    typedef int64_t i64;
    typedef uint64_t u64;
    using std::move, std::string, std::vector, Pack::PackOptionDef;
    using enum Pack::PackOptionType;

    JSON_STRUCT PackOptionDef_glz {
        string identifier, label, description, type;
        generic defaultValue, min, max;
        vector<string> enumValues;
    };

    GLAZE_BIND_START(PackOptionDef_glz)
        GLAZE_BIND_LITERAL(identifier),
        GLAZE_BIND_LITERAL(label),
        GLAZE_BIND_LITERAL(description),
        GLAZE_BIND_LITERAL(type),
        GLAZE_BIND("default", defaultValue),
        GLAZE_BIND_LITERAL(min),
        GLAZE_BIND_LITERAL(max),
        GLAZE_BIND("values", enumValues)
    GLAZE_BIND_END

    GLAZE_DYNAMIC_FROM_START(PackOptionDef)
        PackOptionDef_glz temp;
        parse<JSON>::op<Options>(temp, ctx, it, end);

        result.identifier = move(temp.identifier);
        result.label = move(temp.label);
        result.description = move(temp.description);

        result.destroyUnion();
        if (temp.type == "boolean") {
            result.type = Boolean;
            GLAZE_DYNAMIC_CONSTRAINT(temp.defaultValue.is_boolean(), "Default value for boolean option must be a boolean.")
            result.defaultBool = temp.defaultValue.get<bool>();
        }
        else if (temp.type == "integer") {
            result.type = Integer;
            GLAZE_DYNAMIC_CONSTRAINT(temp.defaultValue.is_number() && temp.min.is_number() && temp.max.is_number(), "Default, min, and max values for integer option must be numbers.")
            result.defaultInt = temp.defaultValue.as<i64>();
            result.intMin = temp.min.as<i64>();
            result.intMax = temp.max.as<i64>();
            GLAZE_DYNAMIC_CONSTRAINT(result.intMin <= result.intMax, "Integer min cannot be greater than integer max.")
        }
        else if (temp.type == "float") {
            result.type = Float;
            GLAZE_DYNAMIC_CONSTRAINT(temp.defaultValue.is_number() && temp.min.is_number() && temp.max.is_number(), "Default, min, and max values for float option must be numbers.")
            result.defaultFloat = temp.defaultValue.as<double>();
            result.floatMin = temp.min.as<double>();
            result.floatMax = temp.max.as<double>();
            GLAZE_DYNAMIC_CONSTRAINT(result.floatMin <= result.floatMax, "Float min cannot be greater than float max.")
        }
        else if (temp.type == "string") {
            result.type = String;
            GLAZE_DYNAMIC_CONSTRAINT(temp.defaultValue.is_string(), "Default value for string option must be a string.")
            new (&result.defaultString) string(temp.defaultValue.get<string>());
        }
        else if (temp.type == "enum") {
            result.type = Enum;
            GLAZE_DYNAMIC_CONSTRAINT(temp.defaultValue.is_string(), "Default value for enum option must be a string.")
            new (&result.defaultEnum) string(temp.defaultValue.get<string>());
            new (&result.enumValues) vector<string>(move(temp.enumValues));
            bool foundDefault = false;
            for (u64 i = 0; i < result.enumValues.size(); i++) if (result.enumValues[i] == result.defaultEnum) {
                foundDefault = true;
                break;
            }
            GLAZE_DYNAMIC_CONSTRAINT(foundDefault, "Default enum value must be one of the values in the enum.")
        }
        else GLAZE_DYNAMIC_CONSTRAINT(false, "Invalid option type.")
    GLAZE_DYNAMIC_FROM_END

    GLAZE_DYNAMIC_TO_START(PackOptionDef)
        PackOptionDef_glz temp;

        temp.identifier = input.identifier;
        temp.label = input.label;
        temp.description = input.description;

        switch (input.type) {
            case Boolean:
                temp.type = "boolean";
                temp.defaultValue = input.defaultBool;
                break;
            case Integer:
                temp.type = "integer";
                temp.defaultValue = input.defaultInt;
                temp.min = input.intMin;
                temp.max = input.intMax;
                break;
            case Float:
                temp.type = "float";
                temp.defaultValue = input.defaultFloat;
                temp.min = input.floatMin;
                temp.max = input.floatMax;
                break;
            case String:
                temp.type = "string";
                temp.defaultValue = input.defaultString;
                break;
            case Enum:
                temp.type = "enum";
                temp.defaultValue = input.defaultEnum;
                temp.enumValues = input.enumValues;
                break;
            case Count:
                break;
        }

        serialize<JSON>::op<Options>(temp, ctx, b, ix);
    GLAZE_DYNAMIC_TO_END
}
