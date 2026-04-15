#pragma once
#include <filesystem>
#include <fstream>
#include <span>
#include <vector>
#include <physfs.h>

#include "../concepts.hpp"

namespace Util::OS {
    typedef uint8_t u8;
    typedef uint64_t u64;
    using std::filesystem::path, std::filesystem::exists, std::filesystem::is_directory, std::filesystem::absolute, std::filesystem::perms, std::error_code, std::ifstream, std::span, std::ofstream, std::streamsize, std::vector;

    [[nodiscard]] inline bool normalize(path& p) noexcept {
        error_code ec;
        p = absolute(p, ec);
        if (ec) return false;
        p = p.lexically_normal();
        p.make_preferred();
        return true;
    }

    [[nodiscard]] inline bool normalize(const path& input, path& result) noexcept {
        error_code ec;
        result = absolute(input, ec);
        if (ec) return false;
        result = result.lexically_normal();
        result.make_preferred();
        return true;
    }

    [[nodiscard]] inline string getU8String(const path& input) noexcept {
    #if defined(__cpp_char8_t)
        const auto utf8 = input.u8string();
        return string(reinterpret_cast<const char*>(utf8.data()), utf8.size());
    #else
        return input.u8string();
    #endif
    }

    [[nodiscard]] inline bool isWritableDirectory(const path& p) noexcept {
        error_code ec;
        path _p = p;
        if (!normalize(_p)) return false;
        if (!exists(_p) || !is_directory(_p)) return false;
        const auto st = status(_p, ec);
        if (ec) return false;
        const auto permissions = st.permissions();
        return (
            ((permissions & perms::owner_write)  != perms::none)
         || ((permissions & perms::group_write)  != perms::none)
         || ((permissions & perms::others_write) != perms::none)
        );
    }

    template <bool physfs, FilePath PathType>
    [[nodiscard]] inline bool readFile(PathType&& path_, vector<u8>& result) noexcept {
        if constexpr (physfs) {
            if (!PHYSFS_isInit()) return false;

            PHYSFS_File* file;
            static_assert(!Equal<PathType, path>, "Path type is not supported for PhysFS. You've probably made a mistake.");
            if constexpr (Equal<PathType, string>) file = PHYSFS_openRead(path_.c_str());
            else if constexpr (Equal<PathType, string_view>) file = PHYSFS_openRead(string(path_).c_str());
            //const char*
            else file = PHYSFS_openRead(path_);
            if (!file) return false;

            const PHYSFS_sint64 fileSize = PHYSFS_fileLength(file);
            if (fileSize < 0) {
                PHYSFS_close(file);
                return false;
            }
            result.resize(static_cast<u64>(fileSize));

            if (PHYSFS_readBytes(file, result.data(), fileSize) < fileSize) {
                PHYSFS_close(file);
                return false;
            }

            PHYSFS_close(file);
            return true;
        }
        else {
            ifstream file(path_, std::ios::binary | std::ios::ate);
            if (!file.is_open()) return false;

            const streamsize fileSize = file.tellg();
            if (fileSize < 0) return false;
            file.seekg(0, std::ios::beg);
            if (!file.good()) return false;
            result.resize(static_cast<u64>(fileSize));

            if (!file.read(reinterpret_cast<char*>(result.data()), fileSize)) return false;
            return true;
        }
    }

    //warning: Make sure you passed in the whole file's data. Passing in a slice of the file's data may cause issues or even vulnerabilities if there is a BOM in the file and the slice starts after the BOM.
    inline void stripBOM(vector<u8>& data) noexcept {
        if (data.size() < 3) return;
        if (data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) data.erase(data.begin(), data.begin() + 3);
    }

    enum struct ExistBehavior : u8 {
        Fail, Overwrite, Append
    };

    //note: It's decided that Physfs will only be used for reading, not writing. So there is no `physfs` template parameter for `writeFile`.
    template <FilePath PathType>
    [[nodiscard]] inline bool writeFile(PathType&& path_, const span<const u8> data, ExistBehavior existBehavior = ExistBehavior::Fail) noexcept {
        if (existBehavior == ExistBehavior::Fail && exists(path_)) return false;
        ofstream file(path_, std::ios::binary | (existBehavior == ExistBehavior::Append ? std::ios::app : std::ios::trunc));
        if (!file.is_open()) return false;
        if (!file.write(reinterpret_cast<const char*>(data.data()), static_cast<streamsize>(data.size()))) {
            file.close();
            return false;
        }
        file.close();
        return true;
    }
}
