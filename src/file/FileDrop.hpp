#pragma once
#include <filesystem>
#include <string>

#include "../debug/Logger.hpp"

namespace FileDrop {
    typedef int32_t i32;
    using std::filesystem::path, std::string;

    //CherryGrove World
    inline void processCGW(const path& _path) noexcept {
        
    }

    //May be world or pack
    inline void processFolder(const path& _path) noexcept {

    }

    //May be world or pack
    inline void processZip(const path& _path) noexcept {

    }

    //CherryGrove Pack
    inline void processCGP(const path& _path) noexcept {

    }

    //CherryGrove Relocatable Gamesave
    inline void processCGR(const path& _path) noexcept {

    }

    inline void processFile(i32 count, const char** paths) noexcept {
        for (i32 i = 0; i < count; i++) {
            const path _path(paths[i]);
            const auto extension =  _path.extension().string();
            if      (extension == ".cgw") processCGW(_path);
            else if (extension == ".cgp") processCGP(_path);
            else if (extension == ".cgr") processCGR(_path);
            else if (extension == ".zip") processZip(_path);
            else if (is_directory(_path)) processFolder(_path);
            else lout << "Unidentified file type: " << paths[i] << endl;
        }
    }
}