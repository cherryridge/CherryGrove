#pragma once
#include <string>
#include <vector>
#include <boost/uuid.hpp>

namespace Util {
    typedef uint64_t u64;
    using std::string, std::vector, boost::uuids::uuid, boost::uuids::string_generator;

    inline static string_generator _gen;

    [[nodiscard]] inline uuid uuidFromString(const string& input) noexcept { return _gen(input); }

    [[nodiscard]] inline vector<uuid> uuidsFromStrings(const vector<string>& input) noexcept {
        vector<uuid> result;
        for (u64 i = 0; i < input.size(); i++) result.push_back(_gen(input[i]));
        return result;
    }
}