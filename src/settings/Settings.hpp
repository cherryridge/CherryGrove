#pragma once
#include <filesystem>
#include <string>

#include "../debug/Fatal.hpp"
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

        inline bool initialized{false};
        inline Latest<Settings> data;
    }

    [[nodiscard]] inline bool isInitialized() noexcept { return detail::initialized; }
    [[nodiscard]] inline const Latest<Settings>& getData() noexcept { return detail::data; }

    //Note: Pre-logger function (maybe called before logger is initialized)
    [[nodiscard]] inline bool loadSettings(bool initial = false) noexcept {
        if (!UmiJSON::readJSONFromFile<Settings>(detail::SETTINGS_FILENAME, detail::data, false)) {
            LOGGER_DYNAMIC_ERR("Failed to parse Settings file.");
            if (initial) Fatal::exit(Fatal::SETTINGS_FAILED_TO_LOAD);
            return false;
        }
        detail::initialized = true;
        return true;
    }

    [[nodiscard]] inline bool saveSettings(const Latest<Settings>& newData) noexcept {
        //todo:
        return true;
    }
}