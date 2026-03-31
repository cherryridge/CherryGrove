#pragma once
#include <cstdint>

namespace Umi {
    typedef uint8_t u8;
    typedef uint32_t u32;

    struct PackExeCtxRef {
        u32 ref{0};

        enum struct Type : u8 {
            Js, Wasm, Lua,
            Count
        } type{Type::Count};
    };
}