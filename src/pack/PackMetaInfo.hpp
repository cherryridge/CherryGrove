#pragma once
#include <string>

#include "../umi/frontend/json/manifest/JSONKind_Manifest.hpp" // IWYU pragma: keep

namespace Pack {
    using std::string, Util::Json::Latest, Util::Json::JSONKind::Manifest;
    
    struct PackMetaInfo {
        Latest<Manifest> manifest;
        string pathStr;
        bool physfs, disabled;
    };
}