#pragma once
#include <filesystem>

#include "../umi/frontend/json/manifest/main.hpp" // IWYU pragma: keep

namespace Pack {
    using std::filesystem::path, Util::Json::Latest, Util::Json::JSONKind::Manifest;
    
    struct PackMetaInfo {
        Latest<Manifest> manifest;
        path path_;
        bool physfs, disabled;
    };
}