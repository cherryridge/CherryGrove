#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

#include "../util/wrappers/uuid.hpp"
#include "PackMetaInfo.hpp"
#include "KnownPack.hpp"

namespace Pack::detail {
    using boost::unordered_flat_map, Util::Wrapper::uuid_JSON;

    inline unordered_flat_map<uuid_JSON, PackMetaInfo> packInfo;
    inline unordered_flat_map<uuid_JSON, KnownPack> knownPacks;

    //nameSpace -> pack UUID in linear topology order
    inline unordered_flat_map<string, vector<uuid_JSON>> shadowingInfo;
}