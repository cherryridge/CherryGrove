#pragma once
#include <cstdint>

#include "../util/wrappers/uuid.hpp"

namespace Pack {
    typedef uint32_t u32;
    using Util::Wrapper::uuid_JSON;

    struct KnownPack {
        uuid_JSON id;
        u32 version;
        bool disabled;
    };
}