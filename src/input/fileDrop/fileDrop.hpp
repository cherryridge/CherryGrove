#pragma once
#include <filesystem>
#include <format>
#include <string>
#include <utility>
#include <SDL3/SDL.h>

#include "../../debug/loggers.hpp"
#include "../../util/os/archiveReader.hpp"
#include "../../util/os/filesystem.hpp"

//1. If it contains `world.cgb` -> WORLD
//2. If it contains `manifest.json` -> PACK, tmb no one ever clashes this
//3. Abstract!!
//  a. WORLD: simulation can CRUD + `open` `close` -> worlds must be IMPORTED (i.e. COPIED) as a FOLDER, we're not modifying a fucking archive!
//  b. PACK: "pack" can get entrypoint file + "extension" and `open` `close
//  c. How about exploring?

namespace InputHandler::FileDrop {
    typedef uint32_t u32;
    using std::filesystem::path, std::format, std::string, std::to_underlying, Util::OS::getU8String, Util::OS::getArType;

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
        if (is_directory(path_)) processFolder(path_);
        const auto extension = getU8String(path_.extension());
        if      (extension == ".cgw") processCGW(path_);
        else if (extension == ".cgp") processCGP(path_);
        else if (extension == ".cgr") processCGR(path_);
        else {
            const auto arType = getArType(path_);
            if (arType == Util::OS::ArchiveType::NotASupportedArchive) lout << "Unidentified file type: " << path_ << nlaf;
            else if (arType == Util::OS::ArchiveType::ExtensionDoesNotMatchContent) lerr << format("File extension {} does not match its content type {}.", extension, static_cast<u32>(to_underlying(arType))) << nlaf;
            else processArchive(path_);
        }
    }
}