#pragma once
#include <string>
#include <boost/container_hash/hash.hpp>

#include "../util/implHashFor.hpp"
#include "../util/string.hpp"

namespace Components {
    using std::string;

    //There is only one invalid state: `colonPos == npos`. Everything else is valid.
    struct NameAndSpace {
        string data;
        size_t colonPos{string::npos};

        [[nodiscard]] explicit NameAndSpace(const string& raw) noexcept : data(raw) {
            colonPos = data.find(':');
            if (!Util::isValidNameAndSpace(raw)) colonPos = string::npos;
        }
        [[nodiscard]] explicit NameAndSpace(const string& nameSpace, const string& name) noexcept : data(nameSpace + ":" + name) {
            if (Util::isValidIdentifier(nameSpace) && Util::isValidIdentifier(name)) colonPos = nameSpace.size();
            else colonPos = string::npos;
        }

        [[nodiscard]] bool isValid() const noexcept { return colonPos != string::npos; }
        operator bool() const noexcept { return isValid(); }

        [[nodiscard]] string getName() const noexcept {
            if (colonPos == string::npos) return "";
            else return data.substr(colonPos + 1);
        }

        [[nodiscard]] string getNameSpace() const noexcept {
            if (colonPos == string::npos) return "";
            else return data.substr(0, colonPos);
        }

        bool operator==(const NameAndSpace& other) const noexcept { return data == other.data && colonPos == other.colonPos; }
    };
}

IMPL_HASH_FOR(Components, NameAndSpace, 555'555'555,
    boost::hash_combine(seed, input.data);
    boost::hash_combine(seed, input.colonPos);
)