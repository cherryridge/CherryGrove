#pragma once
#include <ostream> // IWYU pragma: export

#include "Logger.hpp" // IWYU pragma: export

//Use `output` to reference to the stream, `data` to reference to the data.
//Do not use manipulators in the implementation, as they are different for `std::ostream` and `Debug::Logger`.
#define IMPL_LSHIFT_FOR(type, ...) \
friend std::ostream& operator<<(std::ostream& output, const type& data) noexcept { \
    __VA_ARGS__ \
    return output; \
} \
friend Debug::Logger& operator<<(Debug::Logger& output, const type& data) noexcept { \
    __VA_ARGS__ \
    return output; \
}