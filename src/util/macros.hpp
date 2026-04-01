#pragma once

#define __CONCAT(a, b) a##b

#define IMPL_HASH_FOR(ns, type, _seed, ...)                                     \
namespace ns {                                                                  \
    [[nodiscard]] inline size_t hash_value(const type& input) noexcept {        \
        size_t seed = _seed;                                                    \
        __VA_ARGS__                                                             \
        return seed;                                                            \
    }                                                                           \
}                                                                               \
namespace std {                                                                 \
    template <> struct hash<ns::type> {                                         \
        [[nodiscard]] size_t operator()(const ns::type& input) const noexcept { \
            return ns::hash_value(input);                                       \
        }                                                                       \
    };                                                                          \
}