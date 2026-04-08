#pragma once

#include "../../util/SlotTable.hpp"
#include "TextureAtlas.hpp"
#include "types.hpp"

namespace TexturePool::detail {
    using Util::SlotTable, TexturePool::detail::TextureAtlas, TexturePool::TextureAtlasHandle;

    inline SlotTable<TextureAtlas, TextureAtlasHandle> atlasRegistry;
}