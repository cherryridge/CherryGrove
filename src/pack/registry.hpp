#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/uuid/uuid.hpp>

#include "PackMetaInfo.hpp"
#include "KnownPack.hpp"

namespace Pack::detail {
    using boost::unordered_flat_map, boost::uuids::uuid;

    static unordered_flat_map<uuid, PackMetaInfo> registry;
    static unordered_flat_map<uuid, KnownPack> knownPacks;
}