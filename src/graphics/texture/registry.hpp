#pragma once
#include <bgfx/bgfx.h>

#include "../../util/SlotTable.hpp"

namespace TexturePool::detail {
    using Util::SlotTable, Util::GenerationalHandle;

    struct TextureAtlas {
        bgfx::TextureHandle handle;
        vector<>
    };

    struct TextureHandle {
        GenerationalHandle value;

        bool operator==(const TextureHandle& other) const noexcept { return value == other.value; }
    };

    namespace detail {
        inline SlotTable<TextureAtlas, TextureHandle> registry;

        
    }
}