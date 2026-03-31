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

    [[nodiscard]] inline bool isWritableDirectory(const path& p) noexcept {
        error_code ec;
        path _p = p;
        if (!normalize(_p)) return false;
        if (!exists(_p) || !is_directory(_p)) return false;
        const auto st = status(_p, ec);
        if (ec) return false;
        const auto permissions = st.permissions();
        return (
            ((permissions & perms::owner_write)  != perms::none )
         || ((permissions & perms::group_write)  != perms::none )
         || ((permissions & perms::others_write) != perms::none )
        );
    }

    template <FilePath PathType>
    [[nodiscard]] inline bool readFile(PathType&& path_, vector<u8>& result, bool physfs) noexcept {
        if (physfs) {
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

    enum struct ExistBehavior : u8 {
        Fail, Overwrite, Append
    };

    template <FilePath PathType>
    [[nodiscard]] inline bool writeFile(PathType&& path_, const span<const u8> data, bool physfs, ExistBehavior existBehavior = ExistBehavior::Fail) noexcept {
        if (physfs) {
            if (!PHYSFS_isInit()) return false;

            string pathStr;
            static_assert(!Equal<PathType, path>, "Path type is not supported for PhysFS. You've probably made a mistake.");
            if constexpr (Equal<PathType, string>) pathStr = path_;
            else if constexpr (Equal<PathType, string_view>) pathStr = string(path_);
            //const char*
            else pathStr = string(path_);

            if (existBehavior == ExistBehavior::Fail && PHYSFS_exists(pathStr.c_str())) return false;

            //This cannot be const because `PHYSFS_close` takes a non-const pointer.
            PHYSFS_File* file = existBehavior == ExistBehavior::Append ? PHYSFS_openAppend(pathStr.c_str()) : PHYSFS_openWrite(pathStr.c_str());
            if (file == nullptr) return false;

            const PHYSFS_sint64 bytesWritten = PHYSFS_writeBytes(file, data.data(), static_cast<PHYSFS_uint64>(data.size()));
            PHYSFS_close(file);

            if (bytesWritten < static_cast<PHYSFS_sint64>(data.size())) return false;
            return true;
        }
        else {
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
}