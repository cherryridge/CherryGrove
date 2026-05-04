#pragma once

#include "../../util/SlotTable.hpp"
#include "TextureInfo.hpp"
#include "types.hpp"

namespace TexturePool::internal {
    using Util::SlotTable;

    inline SlotTable<TextureInfo, TextureHandle> textureRegistry;
}