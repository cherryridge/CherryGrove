#pragma once
#include <filesystem>
#include <string>
#include <boost/unordered/unordered_flat_map.hpp>

#include "../debug/Fatal.hpp"
#include "../debug/Logger.hpp"
#include "../settings/pack.hpp"
#include "../settings/Settings.hpp"
#include "../umi/controller.hpp"
#include "../util/os/filesystem.hpp"
#include "packFetcher.hpp"
#include "PackMetaInfo.hpp"
#include "registry.hpp"

namespace Pack {
    typedef uint64_t u64;
    using std::filesystem::current_path, std::string, boost::unordered_flat_map, Util::OS::getU8String, Util::Json::Latest, Util::Json::JSONKind::Settings;

    inline void init() noexcept {
        const Latest<Settings>::Packs& packSettings = Settings::getSettings().packs;
        for (u64 i = 0; i < packSettings.knownPacks.size(); i++) detail::knownPacks.emplace(packSettings.knownPacks[i].id.value(), packSettings.knownPacks[i]);

        getPacksFromPackRoot("packs");
        for (u64 i = 0; i < packSettings.additionalPackRoots.size(); i++) getPacksFromPackRoot(packSettings.additionalPackRoots[i]);

        PackMetaInfo info;
        for (u64 i = 0; i < packSettings.additionalPacks.size(); i++) {
            if (parsePackManifest(packSettings.additionalPacks[i], info)) tryAddingPack(info);
            else lerr << "[Pack] Failed to parse pack: " << packSettings.additionalPacks[i] << endl;
        }
        lout << "[Pack] Found " << detail::registry.size() << " valid packs." << endl;

        if (!Settings::updateKnownPacks(detail::knownPacks)) {
            lerr << "[Pack] Failed to update known packs." << endl;
            Fatal::exit(Fatal::SETTINGS_FAILED_TO_SAVE);
        }

        Umi::init();
    }

    inline void shutdown() noexcept {
        Umi::shutdown();
    }
}