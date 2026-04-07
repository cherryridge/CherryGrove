#pragma once

#include "../../util/SlotTable.hpp"
#include "TextureAtlas.hpp"
#include "TextureInfo.hpp"
#include "types.hpp"

namespace TexturePool::detail::registry {
    using Util::SlotTable, TexturePool::detail::TextureAtlas, TexturePool::detail::TextureInfo, TexturePool::TextureAtlasHandle, TexturePool::TextureHandle;

    inline SlotTable<TextureAtlas, TextureAtlasHandle> atlasRegistry;
    inline SlotTable<TextureInfo, TextureHandle> textureRegistry;
}