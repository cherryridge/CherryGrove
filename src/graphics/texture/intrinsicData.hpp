#pragma once
#include <vector>

#include "../../debug/Fatal.hpp"
#include "../../debug/loggers.hpp"
#include "../../util/os/filesystem.hpp"
#include "RawTexture.hpp"

namespace TexturePool::internal {
    typedef uint8_t u8;
    using std::vector;

    inline RawTexture rawTexture_missing_png, rawTexture_toobig_png;

    inline void readFromIntrinsicFiles() noexcept {
        vector<u8> fileData;
        if (!Util::OS::readFile("assets/textures/toobig.png", fileData)) {
            lerr << "[TexturePool] Failed to read toobig.png." << nlaf;
            Debug::exit(Debug::TEXTUREPOOL_INITIALIZATION_FAILED);
        }
        if (!getRawTexture(fileData, rawTexture_toobig_png)) {
            lerr << "[TexturePool] Failed to load toobig.png." << nlaf;
            Debug::exit(Debug::TEXTUREPOOL_INITIALIZATION_FAILED);
        }
        if (!Util::OS::readFile("assets/textures/missing.png", fileData)) {
            lerr << "[TexturePool] Failed to read missing.png." << nlaf;
            Debug::exit(Debug::TEXTUREPOOL_INITIALIZATION_FAILED);
        }
        if (!getRawTexture(fileData, rawTexture_missing_png)) {
            lerr << "[TexturePool] Failed to load missing.png." << nlaf;
            Debug::exit(Debug::TEXTUREPOOL_INITIALIZATION_FAILED);
        }
    }
}