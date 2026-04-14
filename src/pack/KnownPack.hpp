#pragma once
#include <boost/uuid/uuid.hpp>

namespace Pack {
    typedef uint32_t u32;
    using boost::uuids::uuid;

    struct KnownPack {
        uuid id;
        u32 version;
        bool disabled;
    };
}