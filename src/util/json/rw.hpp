#pragma once
#include <span>
#include <string>
#include <vector>
#include <glaze/glaze.hpp>

#include "../../debug/Logger.hpp"
#include "../concepts.hpp"
#include "../os/filesystem.hpp"

namespace Util::Json {
    typedef uint8_t u8;
    using std::span, std::string, std::vector, glz::opts, glz::read, glz::write, glz::format_error, Util::FilePath, Util::OS::ExistBehavior;

    namespace detail {
        struct ReadOptions : opts {
            //This option makes glaze still validate the JSON structure itself is correct in additional properties.
            //It's really recommended to turn this on for potential confusing attacks, so we just hardcode it.
            bool validate_skipped{true};

            //This option makes glaze error out on trailing whitespaces. Why not?
            bool validate_trailing_whitespace{true};

            bool error_on_const_read{true};
        };

        inline constexpr ReadOptions readOptions{{
            //This is required because we're using `vector<u8>` as the buffer which doesn't have a null terminator.
            .null_terminated = false,

            //We love jsonc. However we cannot use `read_jsonc` directly because it doesn't support custom options.
            .comments = true,

            //This option makes glaze not error out on additional properties.
            .error_on_unknown_keys = false,

            //This option makes glaze error out on missing required properties, literally all of "validation".
            //However, we always need to have a default value for any key anyways, so we can just turn this off and let the default value do the work.
            //But this poses a serious risk of someone forgetting to set a default value on a primitive type or some other shit and we get garbage.
            //But that's a we problem, not a glaze problem.
            //bool error_on_missing_keys{false};
        }};
    }

    template <typename T>
    [[nodiscard]] inline bool readJSON(T& result, const span<const u8> data) noexcept {
        const auto error = read<detail::readOptions>(result, data);
        if (error) {
            lerr << "[Util::Json] Failed to read JSON:\n" << format_error(error, data) << endl;
            return false;
        }
        else return true;
    }

    namespace detail {
        struct WriteOptions : opts {
            //This option does what you think it does.
            u8 indentation_width{4};

            //update: We are forced to do this so objects in arrays doesn't fuck up. We need to pay the price for constantly newlining number arrays though. And this is the default so we don't need to write it :)
            //bool new_lines_in_arrays{true};
        };

        //This option does what you think it does.
        inline constexpr WriteOptions writeOptions{{ .prettify = true }};
    }

    template <typename T>
    [[nodiscard]] inline bool writeJSON(T&& data, vector<u8>& result) noexcept {
        const auto error = write<detail::writeOptions>(data, result);
        if (error) {
            lerr << "[Util::Json] Failed to write JSON:\n" << format_error(error) << endl;
            return false;
        }
        return true;
    }
}