#pragma once

#include "../../util/SlotTable.hpp"
#include "TextureInfo.hpp"
#include "types.hpp"

namespace TexturePool::detail {
    using Util::SlotTable, TexturePool::detail::TextureInfo, TexturePool::TextureHandle;

    inline SlotTable<TextureInfo, TextureHandle> textureRegistry;
}