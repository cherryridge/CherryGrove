#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

#include "../util/wrappers/uuid.hpp"
#include "PackMetaInfo.hpp"
#include "KnownPack.hpp"

namespace Pack::detail {
    using boost::unordered_flat_map, Util::Wrapper::uuid_JSON;

    inline unordered_flat_map<uuid_JSON, PackMetaInfo> registry;
    inline unordered_flat_map<uuid_JSON, KnownPack> knownPacks;
}