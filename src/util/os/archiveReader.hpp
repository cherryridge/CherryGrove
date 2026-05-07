#pragma once
#include <array>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <archive.h>
#include <archive_entry.h>

#include "filesystem.hpp"

namespace Util::OS {
    using std::array, std::filesystem::path, std::filesystem::is_regular_file, std::filesystem::exists, std::string, std::string_view, std::vector;

    enum struct ArchiveType {
        NotASupportedArchive,
        ExtensionDoesNotMatchContent,

        Zip, SevenZ, Rar, TarOnly, TarGz, TarXz, TarZst, Xz, Zst, Lz4,
        //note: gz itself can only contain a single file, so we just don't support it.
        //Gz,

        Count
    };

    [[nodiscard]] inline ArchiveType getArTypeFromExtension(const path& input) noexcept {
        using enum ArchiveType;
        if (!exists(input) || !is_regular_file(input)) return ArchiveType::NotASupportedArchive;
        const auto extension = getSecondExtension(input);
        if      (extension == ".zip") return Zip;
        else if (extension == ".7z")  return SevenZ;
        else if (extension == ".rar") return Rar;
        else if (extension == ".tar") return TarOnly;
        else if (extension == ".tgz" || extension == ".tar.gz") return TarGz;
        else if (extension == ".txz" || extension == ".tar.xz") return TarXz;
        else if (extension == ".tzst" || extension == ".tar.zst") return TarZst;
        else if (extension == ".xz") return Xz;
        else if (extension == ".zst") return Zst;
        else if (extension == ".lz4") return Lz4;
        else return NotASupportedArchive;
    }

    [[nodiscard]] inline ArchiveType getArTypeFromContent(const path& input) noexcept {
        using enum ArchiveType;
        if (!exists(input) || !is_regular_file(input)) return ArchiveType::NotASupportedArchive;
        archive* a = archive_read_new();
        if (!a) return NotASupportedArchive;
        archive_read_support_format_all(a);
        archive_read_support_format_raw(a);
        archive_read_support_filter_all(a);
        const string filename = getU8String(input);
        if (archive_read_open_filename(a, filename.c_str(), 10240) != ARCHIVE_OK) {
            archive_read_free(a);
            return NotASupportedArchive;
        }
        archive_entry* entry = nullptr;
        if (archive_read_next_header(a, &entry) != ARCHIVE_OK) {
            archive_read_free(a);
            return NotASupportedArchive;
        }
        const int formatCode = archive_format(a);
        const bool isTar = (formatCode & ARCHIVE_FORMAT_BASE_MASK) == ARCHIVE_FORMAT_TAR, isRaw = formatCode == ARCHIVE_FORMAT_RAW;

        //Walk filters to find the outer compression. Index convention: iterate all, skip ARCHIVE_FILTER_NONE, take the first real one. (See uncertainty below.)
        int outerFilter = ARCHIVE_FILTER_NONE;
        const int filterCount = archive_filter_count(a);
        for (int i = 0; i < filterCount; i++) {
            const int code = archive_filter_code(a, i);
            if (code != ARCHIVE_FILTER_NONE) {
                outerFilter = code;
                break;
            }
        }

        archive_read_free(a);

        switch (formatCode) {
            case ARCHIVE_FORMAT_ZIP:
                return Zip;
            case ARCHIVE_FORMAT_7ZIP:
                return SevenZ;
            case ARCHIVE_FORMAT_RAR:
            case ARCHIVE_FORMAT_RAR_V5:
                return Rar;
            default: break;
        }
        if (isTar) switch (outerFilter) {
            case ARCHIVE_FILTER_NONE: return TarOnly;
            case ARCHIVE_FILTER_GZIP: return TarGz;
            case ARCHIVE_FILTER_XZ:   return TarXz;
            case ARCHIVE_FILTER_ZSTD: return TarZst;
            default: return NotASupportedArchive;
        }
        if (isRaw) switch (outerFilter) {
            case ARCHIVE_FILTER_XZ:   return Xz;
            case ARCHIVE_FILTER_ZSTD: return Zst;
            case ARCHIVE_FILTER_LZ4:  return Lz4;
            default: return NotASupportedArchive;
        }
        return ArchiveType::NotASupportedArchive;
    }

    //It requires the file's extension to be correct with the file's content to prevent downstream format confusion attack probabilities.
    [[nodiscard]] inline ArchiveType getArType(const path& input) noexcept {
        const ArchiveType extType = getArTypeFromExtension(input);
        if (extType == ArchiveType::NotASupportedArchive) return ArchiveType::NotASupportedArchive;
        const ArchiveType contentType = getArTypeFromContent(input);
        if (contentType == ArchiveType::NotASupportedArchive) return ArchiveType::NotASupportedArchive;
        if (extType != contentType) return ArchiveType::ExtensionDoesNotMatchContent;
        return extType;
    }

    [[nodiscard]] inline bool readFileFromAr(const path& input, const string_view filename, vector<u8>& result) noexcept {
        result.clear();
        if (!exists(input) || !is_regular_file(input)) return false;
        archive* a = archive_read_new();
        if (!a) return false;
        archive_read_support_format_all(a);
        archive_read_support_format_raw(a);
        archive_read_support_filter_all(a);
        const string path_ = getU8String(input);
        if (archive_read_open_filename(a, path_.c_str(), 10240) != ARCHIVE_OK) {
            archive_read_free(a);
            return false;
        }
        archive_entry* entry = nullptr;
        bool found = false, ok = false;
        int headerStatus;
        while ((headerStatus = archive_read_next_header(a, &entry)) == ARCHIVE_OK) {
            const char* entryName = archive_entry_pathname(entry);
            if (!entryName || filename != entryName) {
                archive_read_data_skip(a);
                continue;
            }
            found = true;
            const la_int64_t declaredSize = archive_entry_size(entry);
            if (archive_entry_size_is_set(entry) && declaredSize >= 0) {
                result.resize(static_cast<u64>(declaredSize));
                u64 totalRead = 0;
                while (totalRead < static_cast<u64>(declaredSize)) {
                    const la_ssize_t bytesRead = archive_read_data(a, result.data() + totalRead, static_cast<u64>(declaredSize) - totalRead);
                    if (bytesRead <= 0) break;
                    totalRead += static_cast<u64>(bytesRead);
                }
                ok = totalRead == static_cast<u64>(declaredSize);
            }
            else {
                array<u8, 8192> buffer;
                la_ssize_t bytesRead;
                ok = true;
                while ((bytesRead = archive_read_data(a, buffer.data(), buffer.size())) > 0) result.insert(result.end(), buffer.data(), buffer.data() + bytesRead);
                if (bytesRead < 0) ok = false;
            }
            break;
        }
        archive_read_free(a);
        if (!found || !ok) {
            result.clear();
            return false;
        }
        return true;
    }
}