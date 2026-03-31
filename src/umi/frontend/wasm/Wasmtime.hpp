#pragma once
#include <cstdint>

namespace UmiWasmtime {
    typedef uint32_t u32;
    typedef u32 ModuleID;

    inline constexpr const char* WASMTIME_TEST = "(module\n(func (export \"add\") (param $x i32) (param $y i32) (result i32)\nlocal.get $x\nlocal.get $y\ni32.add\n)\n)";
    inline constexpr ModuleID INVALID_MODULE_HANDLE = 0;

    void init() noexcept;
    void shutdown() noexcept;

    ModuleID addModule(const char* path) noexcept;
}