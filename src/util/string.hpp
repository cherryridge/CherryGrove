#pragma once
#include <string>

namespace Util {
    typedef uint64_t u64;
    using std::string;

    [[nodiscard]] inline bool isValidIdentifier(const string& str) noexcept {
        if (str.empty()) return false;
        const char firstChar = str[0];
        if (!(
            (firstChar >= 'a' && firstChar <= 'z')
         || (firstChar >= 'A' && firstChar <= 'Z')
         || firstChar == '_'
        )) return false;
        for (u64 i = 1; i < str.size(); i++) {
            const char c = str[i];
            if (!(
                (c >= 'a' && c <= 'z')
             || (c >= 'A' && c <= 'Z')
             || (c >= '0' && c <= '9')
             || c == '_'
            )) return false;
        }
        return true;
    }

    [[nodiscard]] inline bool isValidNameAndSpace(const string& str) noexcept {
        const size_t pos = str.find(":");
        if (pos == string::npos) return false;
        const string nameSpace = str.substr(0, pos), identifier = str.substr(pos + 1);
        return isValidIdentifier(nameSpace) && isValidIdentifier(identifier);
    }
}