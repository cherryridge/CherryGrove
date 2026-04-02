#pragma once
#include <filesystem>
#include <string>

#include "../debug/Logger.hpp"
#include "../umi/frontend/json/JSON.hpp"
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

namespace Settings {
    using std::filesystem::exists, std::string, Logger::LOGGER_DYNAMIC_OUT, Logger::LOGGER_DYNAMIC_ERR, Util::Json::Latest, Util::Json::JSONKind::Settings;

    namespace detail {
        inline constexpr const char* SETTINGS_FILENAME = "settings.json";

        inline Latest<Settings> data;
    }

    [[nodiscard]] inline const Latest<Settings>& getSettings() noexcept { return detail::data; }

    //Note: Maybe pre-logger function.
    [[nodiscard]] inline bool loadSettings() noexcept {
        return UmiJSON::readJSONFromFile<Settings, false>(detail::SETTINGS_FILENAME, detail::data);
    }

    //This is usually called automatically by various Setting modification functions so you rarely need to call this manually.
    [[nodiscard]] inline bool saveSettings() noexcept {
        if (!UmiJSON::writeJSONToFile<Settings>(detail::data, detail::SETTINGS_FILENAME, Util::OS::ExistBehavior::Overwrite)) {
            lerr << "[Settings] Failed to write Settings file." << endl;
            return false;
        }
        return true;
    }
}