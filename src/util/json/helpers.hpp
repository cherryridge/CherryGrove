#pragma once
#include <tuple>
#include <type_traits>
#include <utility> // IWYU pragma: keep
#include <vector>
#include <glaze/glaze.hpp>

namespace glz::detail {
    template <class T, class From, class To>
    struct to_json_schema<custom_t<T, From, To>> {
        template <auto Opts>
        static void op(auto& s, auto& defs) noexcept {
            if constexpr (is_invocable_concrete<From> && (std::tuple_size_v<invocable_args_t<From>> >= 2)) to_json_schema<std::decay_t<std::tuple_element_t<1, invocable_args_t<From>>>>::template op<Opts>(s, defs);
            else if constexpr (std::is_invocable_v<To, const T&>) to_json_schema<std::decay_t<std::invoke_result_t<To, const T&>>>::template op<Opts>(s, defs);
            else if constexpr (std::is_invocable_v<To, T&>) to_json_schema<std::decay_t<std::invoke_result_t<To, T&>>>::template op<Opts>(s, defs);
            else s.type = std::vector<std::string_view>{"number", "string", "boolean", "object", "array", "null"};
        }
    };
}

//Make it easy to distinguish and remind ourselves.
#define JSON_STRUCT struct

#define GLAZE_MIMIC(fromType, ...) \
template <> \
struct glz::meta<fromType> { \
    using mimic = __VA_ARGS__; \
};

//---------------------------------------------------------------------------------------

#define GLAZE_DYNAMIC_FROM_START(type) \
template <> \
struct from<JSON, type> { \
    template <auto Options> \
    static void op(type& result, auto&& ctx, auto&& it, auto&& end) noexcept {

#define GLAZE_DYNAMIC_FROM_END }};

#define GLAZE_DYNAMIC_TO_START(type) \
template <> \
struct to<JSON, type> { \
    template <auto Options> \
    static void op(const type& input, is_context auto&& ctx, auto& b, auto& ix) noexcept { \

#define GLAZE_DYNAMIC_TO_END }};

#define GLAZE_DYNAMIC_CONSTRAINT(condition, message) \
if (!(condition)) { \
    ctx.error = error_code::constraint_violated; \
    ctx.custom_error_message = message; \
    return; \
}

//---------------------------------------------------------------------------------------

#define GLAZE_STATIC_CONSTRAINT_BEGIN(type) \
template <> \
struct glz::meta<type> { \
    using T = type; \
    static constexpr auto modify = object(

#define GLAZE_STATIC_CONSTRAINT(field, condition, message) \
#field, custom< \
    [](T& self, decltype(std::declval<T&>().field) field, glz::context& ctx) noexcept { \
        if (!(condition)) { \
            ctx.error = glz::error_code::constraint_violated; \
            ctx.custom_error_message = message; \
            return; \
        } \
        self.field = std::move(field); \
    }, \
    [](const T& self) noexcept -> decltype(auto) { return (self.field); } \
>

#define GLAZE_STATIC_CONSTRAINT_END ); };

//---------------------------------------------------------------------------------------

#define GLAZE_RENAME_START(type) \
template <> \
struct meta<type> { \
    using T = type; \
    static constexpr auto modify = object(

#define GLAZE_RENAME(field, str) str, &T::field

#define GLAZE_RENAME_END ); };

//---------------------------------------------------------------------------------------

#define GLAZE_ENUM_START(type) \
template <> \
struct glz::meta<type> { \
    using enum type; \
    static constexpr auto value = glz::enumerate(

#define GLAZE_ENUM(str, item) str, item

#define GLAZE_ENUM_END ); };