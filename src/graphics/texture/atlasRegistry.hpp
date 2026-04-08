#pragma once

#include "../../util/SlotTable.hpp"
#include "TextureAtlas.hpp"
#include "types.hpp"

namespace TexturePool::internal {
    using Util::SlotTable;

    inline SlotTable<TextureAtlas, TextureAtlasHandle> atlasRegistry;
}