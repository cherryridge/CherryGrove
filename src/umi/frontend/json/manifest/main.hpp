#pragma once

#include "../../../../pack/v1.hpp"
#include "../../../../util/json/formatVersion.hpp"

namespace UmiJSON::Manifest {
    using Util::Json::TypeList, Util::Json::packUpgraders;

    using List = TypeList<
        Pack::Manifest_v1
    >;

    inline constexpr auto upgraders = packUpgraders<
        //
    >();
}

REGISTER_JSON_KIND(Manifest, 1, 1, UmiJSON::Manifest::List, UmiJSON::Manifest::upgraders)