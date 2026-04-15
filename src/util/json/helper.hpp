#pragma once
#include <utility> // IWYU pragma: keep

#define GLAZE_CONSTRAINT_ASSERT(condition, message) \
if (!(condition)) {                                 \
    ctx.error = error_code::constraint_violated;    \
    ctx.custom_error_message = message;             \
    return;                                         \
}

#define GLAZE_CONSTRAINT_DECL(field, condition, message)                           \
#field, custom<                                                                    \
    [](T& self, decltype(std::declval<T&>().field) field, context& ctx) noexcept { \
        if (!(condition)) {                                                        \
            ctx.error = error_code::constraint_violated;                           \
            ctx.custom_error_message = message;                                    \
            return;                                                                \
        }                                                                          \
        self.field = std::move(field);                                             \
    },                                                                             \
    [](const T& self) noexcept -> decltype(auto) { return (self.field); }          \
>