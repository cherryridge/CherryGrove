#pragma once
#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>

#include "../util/json/formatVersion.hpp"
#include "base.hpp"

namespace Pack {
    typedef uint32_t u32;
    using std::string, std::vector, boost::uuids::uuid;

    struct Manifest_v1 {
        u32 formatVersion{INVALID_FORMAT_VERSION};

        uuid id;
        u32 version{0};
        u32 minEngineVersion{0};
        string nameSpace;
        string name{"Unnamed Content Pack"};
        string description{"This content pack has no description."};
        vector<string> authors;
        vector<PackVersionRange> mustBefore;
        vector<PackVersionRange> mustExist;
        vector<PackVersionRange> sendWarning;
        vector<PackVersionRange> sendError;
    };
}