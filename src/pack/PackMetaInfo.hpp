#pragma once
#include <string>

#include "../umi/frontend/json/manifest/main.hpp" // IWYU pragma: keep

namespace Pack {
    using std::string, Util::Json::Latest, Util::Json::JSONKind::Manifest;
    
    struct PackMetaInfo {
        Latest<Manifest> manifest;
        string path_;
        bool physfs, disabled;
    };
}