#pragma once
#include <bgfx/bgfx.h>

#include "../../util/SlotTable.hpp"
#include "TextureAtlas.hpp"
#include "TextureInfo.hpp"
#include "types.hpp"

namespace TexturePool::detail {
    typedef uint32_t u32;
    using Util::SlotTable, Util::GenerationalHandle;

    inline SlotTable<TextureAtlas, TextureAtlasHandle> atlasRegistry;
    inline SlotTable<TextureInfo, TextureHandle> textureRegistry;
}