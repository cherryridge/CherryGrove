#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include "../debug/Fatal.hpp"
#include "../debug/Logger.hpp"
#include "../umi/frontend/json/UmiJSON.hpp"
#include "../util/json/formatVersion.hpp"
#include "../util/json/rw.hpp"
#include "JSONKind_Settings.hpp" // IWYU pragma: keep

namespace Settings {
    typedef uint8_t u8;
    using std::filesystem::exists, std::filesystem::is_regular_file, std::string, std::vector, Logger::LOGGER_DYNAMIC_OUT, Logger::LOGGER_DYNAMIC_ERR, Util::Json::Latest, Util::Json::JSONKind::Settings;

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