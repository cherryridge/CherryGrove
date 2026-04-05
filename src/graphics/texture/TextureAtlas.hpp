#pragma once
#include <vector>
#include <bgfx/bgfx.h>

#include "../../debug/Fatal.hpp"
#include "../../debug/Logger.hpp"
#include "LoneTexture.hpp"
#include "types.hpp"

namespace TexturePool::detail {
    typedef uint32_t u32;
    using std::vector, std::min;

    //The minimum a slot can be, just like a memory unit, and just like a memory unit, we make it 8x8 pixels (not bytes, one pixel is 4 bytes in RGBA8).
    inline constexpr u8 ATLAS_SLOT_UNIT_MAGNITUDE = 3;
    //Probably not guaranteed to be the actual initial size of the atlas in some extreme cases (in which I mean extremely old hardware), but let's try and see.
    inline constexpr u32 ATLAS_INITIAL_EDGE = 1024;
    //The last growth is not guaranteed to be by this factor as we're limited by the device cap, but we will try to grow to the biggest possible size that is divisible by `ATLAS_SLOT_UNIT_SIZE`.
    inline constexpr u8 ATLAS_GROW_FACTOR = 2;

    inline u32 maxTextureSize = 0, initialEdge = 0;

    struct TextureAtlas {
        vector<Rect> freeSlots;
        u32 curWidth{0}, curHeight{0};
        u32 textureCount{0}, maximumTextureCount{0};
        bgfx::TextureHandle internalHandle = BGFX_INVALID_HANDLE;

        explicit TextureAtlas(bool linear = false, u32 maximumTextureCount = 0) noexcept : maximumTextureCount(maximumTextureCount) {
            if (maxTextureSize == 0) {
                const auto* caps = bgfx::getCaps();
                if (caps == nullptr) {
                    lerr << "[TextureAtlas] Failed to query bgfx caps." << endl;
                    Fatal::exit(Fatal::TEXTUREPOOL_INITIALIZATION_FAILED);
                }
                maxTextureSize = caps->limits.maxTextureSize;
                if (maxTextureSize < ATLAS_INITIAL_EDGE) lout << "[TexturePool] Warning: maximum texture size " << maxTextureSize << " is smaller than initial atlas edge " << ATLAS_INITIAL_EDGE << ", constructing the maximum texture at startup." << endl;
                initialEdge = min(maxTextureSize, ATLAS_INITIAL_EDGE);
            }
            internalHandle = bgfx::createTexture2D(
                initialEdge,
                initialEdge,
                false,
                1,
                bgfx::TextureFormat::RGBA8,
                BGFX_TEXTURE_NONE | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT
            );
            if (!bgfx::isValid(internalHandle)) {
                lerr << "[TextureAtlas] Failed to create atlas texture. This is fatal." << endl;
                Fatal::exit(Fatal::TEXTUREPOOL_ATLAS_CREATION_FAILED);
            }
            freeSlots.push_back(Rect { 0, 0, initialEdge, initialEdge });
        }

        //Returns true if the texture was successfully placed in the atlas, false if it cannot fit.
        //Upon failed placement, the caller should allocate a new atlas in the registry.
        [[nodiscard]] bool put(const LoneTexture& texture) noexcept {
            //todo:
        }

        ~TextureAtlas() noexcept {
            if (bgfx::isValid(internalHandle)) bgfx::destroy(internalHandle);
        }
    
    private:
        void grow() noexcept {

        }
    };
}