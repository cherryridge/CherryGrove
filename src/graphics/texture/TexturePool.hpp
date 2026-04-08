#pragma once
#include <string>
#include <vector>

#include "../../debug/Fatal.hpp"
#include "../../debug/Logger.hpp"
#include "../../util/concepts.hpp"
#include "../../util/os/filesystem.hpp"
#include "atlasRegistry.hpp"
#include "intrinsicData.hpp"
#include "RawTexture.hpp"
#include "textureRegistry.hpp"
#include "TextureInfo.hpp"
#include "types.hpp"
#include "uniforms.hpp"

namespace TexturePool {
    typedef uint8_t u8;
    typedef uint32_t u32;
    using std::string, std::vector, std::max, Util::FilePath, Util::OS::readFile;

    inline void init() noexcept {
        detail::initUniforms();
        detail::readFromIntrinsicFiles();
    }

    inline void shutdown() noexcept {
        detail::destroyUniforms();
    }

    [[nodiscard]] inline bool addTexture(const vector<u8>& fileData, TextureHandle& result, bool linear, const string& debug_texture_path = string{}) noexcept {
        detail::RawTexture rawTexture;
        //`getRawTexture` handles logging of errors.
        if (!detail::getRawTexture(fileData, rawTexture, debug_texture_path)) return false;
        const u32 maximumDimension = max(rawTexture.width, rawTexture.height);

        if (maximumDimension > 64) {
            lout << "[TexturePool] Texture " << debug_texture_path << " is large (" << rawTexture.width << "x" << rawTexture.height << "). Using single texture atlas." << endl;
        }

        const TextureHandle tempHandle = detail::textureRegistry.emplace();
        detail::TextureInfo& textureInfo = *detail::textureRegistry.get(tempHandle);
        bool addedToAtlas = false;
        for (auto& atlasEntry : detail::atlasRegistry) if (atlasEntry.put(rawTexture, tempHandle, textureInfo.uvRect, textureInfo.bgfxTextureHandle)) {
            addedToAtlas = true;
            break;
        }

        if (!addedToAtlas) {
            TextureAtlasHandle atlasHandle;
            if (maximumDimension <= 16) atlasHandle = detail::atlasRegistry.emplace(4, linear);
            else if (maximumDimension <= 32) atlasHandle = detail::atlasRegistry.emplace(5, linear);
            else atlasHandle = detail::atlasRegistry.emplace(6, linear);
            auto& atlas = *detail::atlasRegistry.get(atlasHandle);
            if (!atlas.put(rawTexture, tempHandle, textureInfo.uvRect, textureInfo.bgfxTextureHandle)) {
                lerr << "[TexturePool] Failed to add texture " << debug_texture_path << "." << endl;
                return false;
            }
            result = tempHandle;
            return true;
        }
        else {
            textureInfo.width = rawTexture.width;
            textureInfo.height = rawTexture.height;
            result = tempHandle;
            return true;
        }
    }

    template <bool physfs, FilePath PathType>
    [[nodiscard]] inline bool addTexture(PathType&& path_, TextureHandle& result, bool linear) noexcept {
        vector<u8> fileData;
        if (!readFile<physfs>(forward<PathType>(path_), fileData)) {
            lerr << "[TexturePool] Failed to read texture file: " << path_ << "." << endl;
            return false;
        }
        return addTexture(fileData, result, linear, path_);
    }

    inline void useTexture(TextureHandle handle) noexcept {
        const auto* texture = detail::textureRegistry.get(handle);
        if (texture == nullptr) {
            lerr << "[TexturePool] Invalid texture handle " << handle << "." << endl;
            return;
        }
        texture->use();
    }

    [[nodiscard]] inline bool removeTexture(TextureHandle handle) noexcept {
        const auto* textureInfo = detail::textureRegistry.get(handle);
        if (textureInfo == nullptr) {
            lerr << "[TexturePool] Invalid texture handle " << handle << "." << endl;
            return false;
        }
        for (auto& atlasEntry : detail::atlasRegistry) if (atlasEntry.remove(handle)) {
            static_cast<void>(detail::textureRegistry.destroy(handle));
            return true;
        }
        lerr << "[TexturePool] Texture handle " << handle << " is found in the registry but not in any atlas. This should never happen!!" << endl;
        Fatal::exit(Fatal::TEXTUREPOOL_TEXTURE_NOT_IN_ATLAS);
    }
}