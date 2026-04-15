#pragma once
#include <string>
#include <boost/uuid.hpp>

namespace Util {
    typedef uint64_t u64;
    using std::string, boost::uuids::uuid, boost::uuids::string_generator, boost::uuids::to_string;

    namespace detail {
        inline string_generator generator;
    }

    [[nodiscard]] inline uuid uuidFromString(const string& input) noexcept {
        try {
            return detail::generator(input);
        }
        catch (...) {
            return uuid{};
        }
    }

    [[nodiscard]] inline string uuidToString(const uuid& input) noexcept { return to_string(input); }
}