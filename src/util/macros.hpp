#pragma once

#define __CONCAT(a, b) a##b

#define IMPL_HASH_FOR(type, _seed, ...)                                     \
[[nodiscard]] inline size_t hash_value(const type& input) noexcept {        \
    size_t seed = _seed;                                                    \
    __VA_ARGS__                                                             \
    return seed;                                                            \
}                                                                           \
namespace std {                                                             \
    template <> struct hash<type> {                                         \
        [[nodiscard]] size_t operator()(const type& input) const noexcept { \
            return hash_value(input);                                       \
        }                                                                   \
    };                                                                      \
}