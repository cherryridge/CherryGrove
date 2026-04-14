#pragma once
#include <filesystem>
#include <string>

#include "../debug/Fatal.hpp"
#include "../debug/Logger.hpp"
#include "../umi/frontend/json/JSON.hpp"
#include "../util/json/rw.hpp"
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
    using std::filesystem::exists, std::filesystem::is_regular_file, std::string, Logger::LOGGER_DYNAMIC_OUT, Logger::LOGGER_DYNAMIC_ERR, Util::Json::Latest, Util::Json::JSONKind::Settings;

    namespace detail {
        inline constexpr const char* SETTINGS_FILENAME = "settings.json";

        inline Latest<Settings> data;
    }

    [[nodiscard]] inline const Latest<Settings>& getSettings() noexcept { return detail::data; }

    //note: Maybe pre-logger function.
    [[nodiscard]] inline bool loadSettings() noexcept {
        if (exists(detail::SETTINGS_FILENAME) && is_regular_file(detail::SETTINGS_FILENAME)) return UmiJSON::readJSONFromFile<Settings, false>(detail::SETTINGS_FILENAME, detail::data);
        else {
            vector<u8> defaultData;
            if (!Util::Json::writeJSON(detail::data, defaultData)) {
                LOGGER_DYNAMIC_ERR("[Settings] Failed to write default settings.");
                Fatal::exit(Fatal::ISBH_FAILED_TO_WRITE_DEFAULT_SETTINGS);
            }
            if (!Util::OS::writeFile(detail::SETTINGS_FILENAME, defaultData, Util::OS::ExistBehavior::Overwrite)) {
                LOGGER_DYNAMIC_ERR("[Settings] Failed to write default settings to file.");
                Fatal::exit(Fatal::ISBH_FAILED_TO_WRITE_DEFAULT_SETTINGS);
            }
            LOGGER_DYNAMIC_OUT("[Settings] No settings file found. A default one has been created.");
        }
        return true;
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