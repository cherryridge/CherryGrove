#pragma once
#include <string>
#include <boost/container_hash/hash.hpp>

#include "../../../util/string.hpp"

namespace EnttComps::Identity {
    typedef uint64_t u64;
    using std::string, boost::hash_combine;

    //There is only one invalid state: colonPos == npos. Everything else is valid.
    struct NameAndSpace {
        string data;
        size_t colonPos;

        [[nodiscard]] NameAndSpace() noexcept : data(""), colonPos(string::npos) {}
        [[nodiscard]] explicit NameAndSpace(const string& raw) noexcept : data(raw) {
            colonPos = data.find(':');
            if (!Util::isValidNameAndSpace(raw)) colonPos = string::npos;
        }
        [[nodiscard]] explicit NameAndSpace(const string& nameSpace, const string& name) noexcept : data(nameSpace + ":" + name) {
            if (Util::isValidIdentifier(nameSpace) && Util::isValidIdentifier(name)) colonPos = nameSpace.size();
            else colonPos = string::npos;
        }

        [[nodiscard]] bool valid() const noexcept { return colonPos != string::npos; }
        operator bool() const noexcept { return valid(); }

        [[nodiscard]] string getName() const noexcept {
            if (colonPos == string::npos) return "";
            else return data.substr(colonPos + 1);
        }

        [[nodiscard]] string getNameSpace() const noexcept {
            if (colonPos == string::npos) return "";
            else return data.substr(0, colonPos);
        }

        bool operator==(const NameAndSpace& other) const noexcept { return data == other.data; }
    };
}

namespace std {
    template <>
    struct hash<EnttComps::Identity::NameAndSpace> {
        size_t operator()(const EnttComps::Identity::NameAndSpace& ns) const noexcept {
            return hash<string>()(ns.data);
        }
    };
}