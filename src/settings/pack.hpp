#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

#include "../util/wrappers/uuid.hpp"
#include "Settings.hpp"

namespace Settings {
    using boost::unordered_flat_map, Util::Wrapper::uuid_JSON;

    [[nodiscard]] inline bool updateKnownPacks(const unordered_flat_map<uuid_JSON, KnownPack>& knownPacks) noexcept {
        detail::data.packs.knownPacks.clear();
        for (const auto& [id, knownPack] : knownPacks) detail::data.packs.knownPacks.push_back(knownPack);
        return saveSettings();
    }
}