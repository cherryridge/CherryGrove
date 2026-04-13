#pragma once
#include "frontend/lua/Lua.hpp"
#include "frontend/js/JS.hpp"
#include "frontend/wasm/WASM.hpp"

namespace Umi {
    inline void init() noexcept {
        UmiLua::init();
        UmiJS::init();
        UmiWASM::init();
    }

    inline void shutdown() noexcept {
        UmiWASM::shutdown();
        UmiJS::shutdown();
        UmiLua::shutdown();
    }

    inline void loadPacks() noexcept {
        
    }
}