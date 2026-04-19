#pragma once
#include <utility> // IWYU pragma: keep

//Make it easy to distinguish and remind ourselves.
#define JSON_STRUCT struct

//---------------------------------------------------------------------------------------

#define GLAZE_DYNAMIC_FROM_START(type)                                             \
template <>                                                                        \
    struct from<JSON, type> {                                                      \
        template <auto Options>                                                    \
        static void op(type& result, auto&& ctx, auto&& it, auto&& end) noexcept {

#define GLAZE_DYNAMIC_FROM_END }};

#define GLAZE_DYNAMIC_TO_START(type)                                                           \
template <>                                                                                    \
    struct to<JSON, type> {                                                                    \
        template <auto Options>                                                                \
        static void op(const type& input, is_context auto&& ctx, auto& b, auto& ix) noexcept { \

#define GLAZE_DYNAMIC_TO_END }};

#define GLAZE_DYNAMIC_CONSTRAINT(condition, message) \
if (!(condition)) {                                  \
    ctx.error = error_code::constraint_violated;     \
    ctx.custom_error_message = message;              \
    return;                                          \
}

//---------------------------------------------------------------------------------------

#define GLAZE_STATIC_CONSTRAINT_BEGIN(type)    \
template <>                                    \
    struct glz::meta<type> {                   \
        using T = type;                        \
        static constexpr auto modify = object(

#define GLAZE_STATIC_CONSTRAINT(field, condition, message)                              \
#field, custom<                                                                         \
    [](T& self, decltype(std::declval<T&>().field) field, glz::context& ctx) noexcept { \
        if (!(condition)) {                                                             \
            ctx.error = glz::error_code::constraint_violated;                           \
            ctx.custom_error_message = message;                                         \
            return;                                                                     \
        }                                                                               \
        self.field = std::move(field);                                                  \
    },                                                                                  \
    [](const T& self) noexcept -> decltype(auto) { return (self.field); }               \
>

#define GLAZE_STATIC_CONSTRAINT_END ); };

//---------------------------------------------------------------------------------------

#define GLAZE_BIND_START(type)                \
template <>                                   \
    struct meta<type> {                       \
        using T = type;                       \
        static constexpr auto value = object(

#define GLAZE_BIND(str, field) str, &T::field
#define GLAZE_BIND_LITERAL(str) #str, &T::str

#define GLAZE_BIND_END ); };

//---------------------------------------------------------------------------------------

#define GLAZE_ENUM_START(type)                    \
template <>                                       \
struct glz::meta<type> {                          \
    using enum type;                              \
    static constexpr auto value = glz::enumerate(

#define GLAZE_ENUM(str, item) str, item

#define GLAZE_ENUM_END ); };
