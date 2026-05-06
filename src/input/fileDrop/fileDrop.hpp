#pragma once
#include <filesystem>
#include <string>
#include <SDL3/SDL.h>

#include "../../debug/Logger.hpp"
#include "../../util/os/filesystem.hpp"

//todo: Implement this as an input component.

namespace InputHandler::FileDrop {
    typedef int32_t i32;
    using std::filesystem::path, std::string, Util::OS::getU8String;

    //CherryGrove World
    inline void processCGW(const path& path_) noexcept {
        
    }

    //May be world or pack
    inline void processFolder(const path& path_) noexcept {

    }

    //May be world or pack
    inline void processArchive(const path& path_) noexcept {

    }

    //CherryGrove Pack
    inline void processCGP(const path& path_) noexcept {

    }

    //CherryGrove Relocatable Gamesave
    inline void processCGR(const path& path_) noexcept {

    }

    inline void processTrigger(const SDL_Event& event) noexcept {
        const path path_(event.drop.data);
        //todo: tolower and check for secondary extensions (e.g. .tar.gz, .tar.zst)
        const auto extension = getU8String(path_.extension());
        if      (extension == ".cgw") processCGW(path_);
        else if (extension == ".cgp") processCGP(path_);
        else if (extension == ".cgr") processCGR(path_);
        else if (
            extension == ".zip"
         || extension == ".7z"
         || extension == ".rar"
         || extension == ".tar"
         || extension == ".gz" // .tar.gz
         || extension == ".tgz"
         || extension == ".xz" // .tar.xz
         || extension == ".txz"
         || extension == ".lz4" // .tar.lz4
         || extension == ".zst" // .tar.zst
         || extension == ".tzst"
        ) processArchive(path_);
        else if (is_directory(path_)) processFolder(path_);
        else lout << "Unidentified file type: " << getU8String(path_) << endl;
    }
}