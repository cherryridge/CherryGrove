#pragma once
#include <string>

namespace Util {
    typedef uint64_t u64;
    using std::string;

    inline void toLower(string& str) noexcept {
        for (char& c : str) if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    }

    inline void toUpper(string& str) noexcept {
        for (char& c : str) if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
    }
}