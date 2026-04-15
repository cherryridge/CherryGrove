#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/uuid.hpp>

#include "PackMetaInfo.hpp"
#include "KnownPack.hpp"

namespace Pack::detail {
    using boost::unordered_flat_map, boost::uuids::uuid;

    inline unordered_flat_map<uuid, PackMetaInfo> registry;
    inline unordered_flat_map<uuid, KnownPack> knownPacks;
}