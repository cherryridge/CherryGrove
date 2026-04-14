#pragma once
#include <bgfx/bgfx.h>

#include "size.hpp"

namespace Renderer {
    inline void prepareForFrame() noexcept {
        respondToResize();
    #if DEBUG
        bgfx::setDebug(BGFX_DEBUG_STATS | BGFX_DEBUG_PROFILER | BGFX_DEBUG_TEXT);
    #endif
    }
}