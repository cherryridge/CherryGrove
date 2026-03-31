#pragma once
#include "frontend/lua/LuaJIT.hpp"
#include "frontend/js/V8.hpp"
#include "frontend/wasm/Wasmtime.hpp"

namespace Umi {
    inline void init() noexcept {
        UmiLuau::init();
        UmiV8::init();
        UmiWasmtime::init();
    }

    inline void shutdown() noexcept {
        UmiWasmtime::shutdown();
        UmiV8::shutdown();
        UmiLuau::shutdown();
    }
}