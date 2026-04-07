#pragma once
#include <algorithm>
#include <vector>
#include <bgfx/bgfx.h>

#include "../../debug/Fatal.hpp"
#include "../../debug/Logger.hpp"
#include "RawTexture.hpp"
#include "types.hpp"

namespace TexturePool::detail {
    typedef uint8_t u8;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::vector, std::lower_bound, std::min;

    //Initial number of slots in the atlas. Slots grow alongside the u edge (width) of the atlas, and the v edge (height) grows when the u edge is full. The initial number of slots might not be met if the device cap is too small, in which case a warning will be logged.
    inline constexpr u32 ATLAS_INITIAL_CAPACITY = 256;
    //The last growth is not guaranteed to be by this factor as we're limited by the device cap, but we will try to grow to the biggest possible size that is divisible by `ATLAS_SLOT_UNIT_SIZE`.
    inline constexpr u8 ATLAS_GROW_FACTOR = 2;

    inline u32 bgfxMaxTextureSize = 0;

    struct TextureAtlas {
        vector<u8> data;
        vector<TextureHandle> residentTextures;
        vector<u32> freeIndexes;
        u32 uSlots{0}, vSlots{0};
        bgfx::TextureHandle internalHandle = BGFX_INVALID_HANDLE;
        const u8 slotSizeMag;
        const bool linear, singleTexture;

        [[nodiscard]] TextureAtlas(u8 slotSizeMag, bool linear) noexcept : slotSizeMag(slotSizeMag), linear(linear), singleTexture(false) {
            queryMaxTextureSize();

            const u32 maxSlotsPerEdge = bgfxMaxTextureSize >> slotSizeMag;
            if (maxSlotsPerEdge * maxSlotsPerEdge > ATLAS_INITIAL_CAPACITY) lout << "[TexturePool] Warning: maximum allowed slots " << maxSlotsPerEdge * maxSlotsPerEdge << " is bigger than `ATLAS_INITIAL_SLOTS` " << ATLAS_INITIAL_CAPACITY << ", constructing the maximum available slots at startup." << endl;
            const u32 finalInitialCapacity = min(ATLAS_INITIAL_CAPACITY, maxSlotsPerEdge * maxSlotsPerEdge);
            uSlots = finalInitialCapacity > maxSlotsPerEdge ? maxSlotsPerEdge : finalInitialCapacity;
            vSlots = (finalInitialCapacity - 1) / maxSlotsPerEdge + 1;

            internalHandle = bgfx::createTexture2D(
                uSlots << slotSizeMag, vSlots << slotSizeMag,
                false, 1,
                bgfx::TextureFormat::RGBA8,
                linear ? BGFX_SAMPLER_NONE : (BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT),
                //Passing `nullptr` to mark the texture mutable.
                nullptr
            );

            if (!bgfx::isValid(internalHandle)) {
                lerr << "[TextureAtlas] Failed to create atlas texture. This is fatal." << endl;
                Fatal::exit(Fatal::TEXTUREPOOL_ATLAS_CREATION_FAILED);
            }

            data.resize(static_cast<u64>(uSlots) * uSlots * 4);
        }

        [[nodiscard]] TextureAtlas(u8 slotSizeMag, bool linear, const RawTexture& rawTexture, TextureHandle handle, UVRect& res_UVRect, bgfx::TextureHandle& res_bgfxHandle) noexcept : slotSizeMag(slotSizeMag), linear(linear), singleTexture(true) {
            queryMaxTextureSize();

            if (rawTexture.width > bgfxMaxTextureSize || rawTexture.height > bgfxMaxTextureSize) {
                lerr << "[TextureAtlas] Failed to create single texture atlas: the provided texture is too big to fit in the maximum texture size of the device." << endl;
                //todo: return the `toobig.png` texture handle.
            }

            internalHandle = bgfx::createTexture2D(
                rawTexture.width, rawTexture.height,
                false, 1,
                bgfx::TextureFormat::RGBA8,
                linear ? BGFX_SAMPLER_NONE : (BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT),
                bgfx::copy(rawTexture.pixels.data(), static_cast<u32>(rawTexture.pixels.size()))
            );

            if (!bgfx::isValid(internalHandle)) {
                lerr << "[TextureAtlas] Failed to create atlas texture. This is fatal." << endl;
                Fatal::exit(Fatal::TEXTUREPOOL_ATLAS_CREATION_FAILED);
            }

            residentTextures.push_back(handle);
            res_UVRect = {0.0f, 0.0f, 1.0f, 1.0f};
            res_bgfxHandle = internalHandle;
        }

        //Returns true if the texture was successfully placed in the atlas, false if it cannot fit.
        //Upon failed placement, the caller should allocate a new atlas in the registry.
        [[nodiscard]] bool put(const RawTexture& rawTexture, TextureHandle handle, UVRect& res_UVRect, bgfx::TextureHandle& res_bgfxHandle) noexcept {
            if (singleTexture) {
                lerr << "[TextureAtlas] Failed to put texture: this is a single texture atlas!" << endl;
                return false;
            }
            //todo:
            //O(n) insertion for later.
            residentTextures.insert(lower_bound(residentTextures.begin(), residentTextures.end(), handle), handle);
        }

        [[nodiscard]] bool remove(const TextureHandle& handle) noexcept {
            if (singleTexture) {
                lerr << "[TextureAtlas] Failed to remove texture: this is a single texture atlas! Drop the atlas itself from the registry instead." << endl;
                return false;
            }
            if (!contains(handle)) {
                lerr << "[TextureAtlas] Failed to remove texture: the provided handle is not in this atlas." << endl;
                return false;
            }
            //todo:
        }

        //Yes, Handles can be sorted (they are just two u32s!) and will be sorted to save time on hot path.
        [[nodiscard]] bool contains(const TextureHandle& handle) const noexcept {
            return lower_bound(residentTextures.begin(), residentTextures.end(), handle) != residentTextures.end();
        }

        [[nodiscard]] u64 getCapacity() const noexcept { return uSlots * vSlots; }
        [[nodiscard]] u64 getSize() const noexcept { return residentTextures.size(); }

        ~TextureAtlas() noexcept { destroy(); }

    private:
        void destroy() noexcept { if (bgfx::isValid(internalHandle)) bgfx::destroy(internalHandle); }

        void queryMaxTextureSize() noexcept {
            if (bgfxMaxTextureSize == 0) {
                const auto* caps = bgfx::getCaps();
                if (caps == nullptr) {
                    lerr << "[TextureAtlas] Failed to query bgfx caps." << endl;
                    Fatal::exit(Fatal::TEXTUREPOOL_INITIALIZATION_FAILED);
                }
                bgfxMaxTextureSize = caps->limits.maxTextureSize;
                lout << "[TexturePool] Queried maximum texture size: " << bgfxMaxTextureSize << "." << endl;
            }
        }

        [[nodiscard]] bool grow() noexcept {
            if (singleTexture) {
                lerr << "[TextureAtlas] Failed to grow atlas: single texture atlas!" << endl;
                return false;
            }
            
        }
    };
}