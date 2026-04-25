#pragma once

#include "../util/json/formatVersion.hpp"
#include "v1.hpp"

namespace Settings::detail {
    using Util::Json::packUpgraders, Util::Json::TypeList;

    using Types = TypeList<
        Settings_v1
    >;

    inline constexpr auto upgraders = packUpgraders<
        //
    >();
}

REGISTER_JSON_KIND(Settings, 1, 1, Settings::detail::Types, Settings::detail::upgraders)