#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/uuid/uuid.hpp>

#include "Settings.hpp"

namespace Settings {
    using boost::unordered_flat_map, boost::uuids::uuid;

    [[nodiscard]] inline bool updateKnownPacks(const unordered_flat_map<uuid, KnownPack>& knownPacks) noexcept {
        detail::data.packs.knownPacks.clear();
        for (const auto& [id, knownPack] : knownPacks) detail::data.packs.knownPacks.push_back(knownPack);
        return saveSettings();
    }
}