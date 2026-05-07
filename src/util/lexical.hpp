#pragma once
#include <string_view>

namespace Util {
    typedef uint8_t u8;
    typedef uint64_t u64;
    using std::string_view;

    [[nodiscard]] inline bool isWhitespace(char c) noexcept {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    [[nodiscard]] inline bool isValidIdentifierStart(char c) noexcept {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    [[nodiscard]] inline bool isValidIdentifierPart(char c) noexcept {
        return isValidIdentifierStart(c) || (c >= '0' && c <= '9');
    }

    [[nodiscard]] inline bool isValidIdentifier(const string_view str) noexcept {
        if (str.size() == 0) return false;
        for (u64 i = 0; i < str.size(); i++) if (
            (i == 0 && !isValidIdentifierStart(str[i]))
         || !isValidIdentifierPart(str[i])
        ) return false;
        return true;
    }

    [[nodiscard]] inline bool isHexDigit(char c) noexcept {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }

    [[nodiscard]] inline u8 getHexValue(char c) noexcept {
        if (c >= '0' && c <= '9') return c - '0';
        else if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        else return 0; //Should not happen if we only call this on valid hex digits.
    }

    [[nodiscard]] inline bool isOctalDigit(char c) noexcept {
        return c >= '0' && c <= '7';
    }

    [[nodiscard]] inline bool isValidNameAndSpace(const string_view str) noexcept {
        const size_t pos = str.find(':');
        if (pos == string_view::npos) return false;
        const string_view nameSpace = str.substr(0, pos), identifier = str.substr(pos + 1);
        return isValidIdentifier(nameSpace) && isValidIdentifier(identifier);
    }
}