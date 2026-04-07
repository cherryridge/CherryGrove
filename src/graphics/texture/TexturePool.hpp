#pragma once
#include <vector>

#include "../../util/concepts.hpp"
#include "../../util/os/filesystem.hpp"
#include "RawTexture.hpp"
#include "registry.hpp"
#include "types.hpp"
#include "uniforms.hpp"

namespace TexturePool {
    typedef uint8_t u8;
    using std::vector, Util::FilePath, Util::OS::readFile;

    inline void init() noexcept {
        detail::initUniforms();
    }

    inline void shutdown() noexcept {
        detail::destroyUniforms();
    }

    template <bool physfs, FilePath PathType>
    [[nodiscard]] inline bool addTexture(PathType&& path_, TextureHandle& result) noexcept {
        vector<u8> fileData;
        if (!readFile<physfs>(path_, fileData)) {
            lerr << "[TexturePool] Failed to read texture file " << path_ << "." << endl;
            return false;
        }
        detail::RawTexture rawTexture;
        if (!detail::getRawTexture(fileData, rawTexture, path_)) {
            lerr << "[TexturePool] Failed to load texture from file data of " << path_ << "." << endl;
            return false;
        }
        //todo: add the texture to an atlas.
        for (auto& atlasEntry : detail::atlasRegistry) {
            if (atlasEntry.put(rawTexture, uvRect)) {

            }
        }
    }

    inline void useTexture(const TextureHandle& handle) noexcept {
        const auto* texture = detail::textureRegistry.get(handle);
        if (texture == nullptr) {
            lerr << "[TexturePool] Invalid texture handle " << handle << "." << endl;
            return;
        }
        texture->use();
    }
}