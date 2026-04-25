#pragma once
#include <string>
#include <string_view>

namespace Boot::Focus::detail {
    typedef uint8_t u8;
    typedef uint64_t u64;
    using std::string_view, std::string, std::to_string;

    [[nodiscard]] inline string hashPath(const string_view input) noexcept {
        u64 hash = 14695981039346656037ull;
        for (const u8 c : input) {
            const u8 normalized = c == '\\' ? '/' : (c >= 'A' && c <= 'Z' ? c + 'a' - 'A' : c);
            hash ^= normalized;
            hash *= 1099511628211ull;
        }
        return to_string(hash);
    }
}