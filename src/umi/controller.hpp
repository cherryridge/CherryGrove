#pragma once
#include "frontend/lua/UmiLua.hpp"
#include "frontend/js/UmiJS.hpp"
#include "frontend/wasm/UmiWASM.hpp"

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