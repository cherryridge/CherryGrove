#pragma once
#include <algorithm>
#include <compare>
#include <cstring>
#include <vector>
#include <bgfx/bgfx.h>

#include "../../debug/Fatal.hpp"
#include "../../debug/Logger.hpp"
#include "intrinsicData.hpp"
#include "RawTexture.hpp"
#include "TextureInfo.hpp"
#include "textureRegistry.hpp"
#include "types.hpp"

namespace TexturePool::internal {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::vector, std::lower_bound, std::min, std::memcpy, std::strong_ordering;

    //The last growth is not guaranteed to be by this factor as we're limited by the device cap, but we will try to grow to the biggest possible size.
    inline constexpr u8 ATLAS_GROW_FACTOR = 2;

    inline u32 bgfxMaxTextureSize = 0;

    struct ResidentEntry {
        TextureHandle handle;
        u32 slotIndex;

        bool operator==(const ResidentEntry& other) const noexcept { return handle == other.handle; }
        friend bool operator==(const ResidentEntry& this_, const TextureHandle& other) noexcept { return this_.handle == other; }

        strong_ordering operator<=>(const ResidentEntry& other) const noexcept { return handle <=> other.handle; }
        friend strong_ordering operator<=>(const ResidentEntry& this_, const TextureHandle& other) noexcept { return this_.handle <=> other; }
    };

    struct TextureAtlas {
        vector<u8> data;
        vector<ResidentEntry> residentTextures;
        vector<u32> freeIndexes;
        bgfx::TextureHandle internalHandle = BGFX_INVALID_HANDLE;
        u32 vSlots;
        const u8 slotSizeMag;
        const bool linear, singleTexture;

        //Defaults to one row of slots at the beginning.
        [[nodiscard]] TextureAtlas(u8 slotSizeMag, bool linear) noexcept : slotSizeMag(slotSizeMag), linear(linear), singleTexture(false), vSlots(1) {
            queryMaxTextureSize();

            internalHandle = bgfx::createTexture2D(
                bgfxMaxTextureSize, 1ull << slotSizeMag,
                false, 1,
                bgfx::TextureFormat::RGBA8,
                linear ? BGFX_SAMPLER_NONE : (BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT),
                //Passing `nullptr` to mark the texture mutable.
                nullptr
            );

            if (!bgfx::isValid(internalHandle)) {
                lerr << "[TexturePool] Failed to create atlas texture. This is fatal." << endl;
                Fatal::exit(Fatal::TEXTUREPOOL_ATLAS_CREATION_FAILED);
            }

            data.resize(static_cast<u64>(bgfxMaxTextureSize) * (1ull << slotSizeMag) * 4);
        }

        [[nodiscard]] TextureAtlas(u8 slotSizeMag, bool linear, const RawTexture& rawTexture_, TextureHandle handle, UVRect& res_UVRect, bgfx::TextureHandle& res_bgfxHandle) noexcept : slotSizeMag(slotSizeMag), linear(linear), singleTexture(true) {
            queryMaxTextureSize();

            const bool tooBig = rawTexture_.width > bgfxMaxTextureSize || rawTexture_.height > bgfxMaxTextureSize;
            if (tooBig) lerr << "[TexturePool] Failed to create single texture atlas: the provided texture is too big to fit in the maximum texture size of the device." << endl;
            const auto& rawTexture = tooBig ? rawTexture_toobig_png : rawTexture_;

            internalHandle = bgfx::createTexture2D(
                static_cast<u16>(rawTexture.width), static_cast<u16>(rawTexture.height),
                false, 1,
                bgfx::TextureFormat::RGBA8,
                linear ? BGFX_SAMPLER_NONE : (BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT),
                bgfx::copy(rawTexture.pixels.data(), static_cast<u32>(rawTexture.pixels.size()))
            );

            if (!bgfx::isValid(internalHandle)) {
                lerr << "[TexturePool] Failed to create atlas texture. This is fatal." << endl;
                Fatal::exit(Fatal::TEXTUREPOOL_ATLAS_CREATION_FAILED);
            }

            residentTextures.emplace_back(handle, 0);
            res_UVRect = {0.0f, 0.0f, 1.0f, 1.0f};
            res_bgfxHandle = internalHandle;
        }

        //Returns true if the texture was successfully placed in the atlas, false if it cannot fit.
        //Upon failed placement, the caller should allocate a new atlas in the registry.
        [[nodiscard]] bool put(const RawTexture& rawTexture, TextureHandle handle, UVRect& res_UVRect, bgfx::TextureHandle& res_bgfxHandle) noexcept {
            if (singleTexture) {
                lerr << "[TexturePool] Failed to put texture: this is a single texture atlas!" << endl;
                return false;
            }
            if (contains(handle)) {
                lerr << "[TexturePool] Failed to put texture: the provided handle is already in this atlas." << endl;
                return false;
            }
            if (rawTexture.width > (1ull << slotSizeMag) || rawTexture.height > (1ull << slotSizeMag)) {
                lout << "[TexturePool] Texture is too big (" << rawTexture.width << "x" << rawTexture.height << ") to fit in this atlas (slot size: " << (1ull << slotSizeMag) << "x" << (1ull << slotSizeMag) << ")." << endl;
                return false;
            }

            u32 index;
            if (freeIndexes.empty()) {
                if (getTextureCount() == getCapacity() && !grow()) {
                    lerr << "[TexturePool] Failed to put texture: atlas is full and cannot grow." << endl;
                    return false;
                }
                index = getTextureCount();
            }
            else {
                index = freeIndexes.back();
                freeIndexes.pop_back();
            }

            const u32
                uSlots = bgfxMaxTextureSize >> slotSizeMag,
                slotU = index % uSlots,
                slotV = index / uSlots,
                pixelU = slotU << slotSizeMag,
                pixelV = slotV << slotSizeMag;

            // Copy pixel data row-by-row into the atlas data buffer.
            for (u64 py = 0; py < rawTexture.height; py++) {
                const u64
                    srcOffset = py * rawTexture.width * 4,
                    dstOffset = ((py + pixelV) * bgfxMaxTextureSize + pixelU) * 4;
                memcpy(data.data() + dstOffset, rawTexture.pixels.data() + srcOffset, static_cast<u64>(rawTexture.width) * 4);
            }

            // Update GPU-side texture.
            bgfx::updateTexture2D(
                internalHandle, 0, 0,
                static_cast<u16>(pixelU), static_cast<u16>(pixelV),
                static_cast<u16>(rawTexture.width), static_cast<u16>(rawTexture.height),
                bgfx::copy(rawTexture.pixels.data(), static_cast<u32>(rawTexture.pixels.size()))
            );

            // Calculate normalized UV coordinates.
            const float atlasHeight = static_cast<float>(vSlots << slotSizeMag);
            res_UVRect = {
                static_cast<float>(pixelU) / bgfxMaxTextureSize,
                static_cast<float>(pixelV) / atlasHeight,
                static_cast<float>(pixelU + rawTexture.width) / bgfxMaxTextureSize,
                static_cast<float>(pixelV + rawTexture.height) / atlasHeight
            };
            res_bgfxHandle = internalHandle;

            //O(n) insertion for later `contains` binary search. This is acceptable since `put` is not on the hot path.
            residentTextures.insert(lower_bound(residentTextures.begin(), residentTextures.end(), handle), ResidentEntry{handle, index});
            return true;
        }

        //It checks if the handle exists. There is no need to call `contains` before `remove`.
        [[nodiscard]] bool remove(const TextureHandle& handle) noexcept {
            if (singleTexture) {
                lerr << "[TexturePool] Failed to remove texture: this is a single texture atlas! Drop the atlas itself from the registry instead." << endl;
                return false;
            }
            const auto it = lower_bound(residentTextures.begin(), residentTextures.end(), handle);
            if (it == residentTextures.end() || *it != handle) return false;
            freeIndexes.push_back(it->slotIndex);
            residentTextures.erase(it);
            return true;
        }

        //Yes, Handles can be sorted (they are just two u32s!) and will be sorted to save time on hot path.
        [[nodiscard]] bool contains(const TextureHandle& handle) const noexcept {
            const auto it = lower_bound(residentTextures.begin(), residentTextures.end(), handle);
            return it != residentTextures.end() && *it == handle;
        }

        [[nodiscard]] u64 getCapacity() const noexcept { return static_cast<u64>(bgfxMaxTextureSize >> slotSizeMag) * vSlots; }
        [[nodiscard]] u64 getTextureCount() const noexcept { return residentTextures.size(); }

        ~TextureAtlas() noexcept { destroy(); }

    private:
        void destroy() noexcept { if (bgfx::isValid(internalHandle)) bgfx::destroy(internalHandle); }

        void queryMaxTextureSize() noexcept {
            if (bgfxMaxTextureSize == 0) {
                const auto* caps = bgfx::getCaps();
                if (caps == nullptr) {
                    lerr << "[TexturePool] Failed to query bgfx caps." << endl;
                    Fatal::exit(Fatal::TEXTUREPOOL_INITIALIZATION_FAILED);
                }
                bgfxMaxTextureSize = caps->limits.maxTextureSize;
                lout << "[TexturePool] Queried maximum texture size: " << bgfxMaxTextureSize << "." << endl;
            }
        }

        [[nodiscard]] bool grow() noexcept {
            if (singleTexture) {
                lerr << "[TexturePool] Failed to grow atlas: single texture atlas!" << endl;
                return false;
            }

            const u32 maxVSlots = bgfxMaxTextureSize >> slotSizeMag;
            if (vSlots >= maxVSlots) {
                lerr << "[TexturePool] Failed to grow atlas: already at maximum capacity." << endl;
                return false;
            }

            const u32
                newVSlots = min(static_cast<u32>(vSlots * ATLAS_GROW_FACTOR), maxVSlots),
                oldHeight = vSlots << slotSizeMag,
                newHeight = newVSlots << slotSizeMag;

            data.resize(static_cast<u64>(bgfxMaxTextureSize) * newHeight * 4);

            // Destroy old GPU texture and create a new, bigger one.
            destroy();
            internalHandle = bgfx::createTexture2D(
                static_cast<u16>(bgfxMaxTextureSize), static_cast<u16>(newHeight),
                false, 1,
                bgfx::TextureFormat::RGBA8,
                linear ? BGFX_SAMPLER_NONE : (BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT),
                nullptr
            );

            if (!bgfx::isValid(internalHandle)) {
                lerr << "[TexturePool] Failed to grow atlas: texture creation failed. This is fatal." << endl;
                Fatal::exit(Fatal::TEXTUREPOOL_ATLAS_CREATION_FAILED);
            }

            // Re-upload existing pixel data to the new texture.
            bgfx::updateTexture2D(
                internalHandle, 0, 0,
                0, 0,
                static_cast<u16>(bgfxMaxTextureSize), static_cast<u16>(oldHeight),
                bgfx::copy(data.data(), static_cast<u32>(static_cast<u64>(bgfxMaxTextureSize) * oldHeight * 4))
            );

            vSlots = newVSlots;

            // Update every resident texture's UVRect (V coordinates rescale) and bgfx handle.
            const float vScale = static_cast<float>(oldHeight) / static_cast<float>(newHeight);
            for (const auto& entry : residentTextures) {
                auto* info = textureRegistry.get(entry.handle);
                if (info == nullptr) continue;
                info->bgfxTextureHandle = internalHandle;
                info->uvRect.v0 *= vScale;
                info->uvRect.v1 *= vScale;
            }

            return true;
        }
    };
}