#pragma once
#include <cstdint>
#include <boost/unordered/unordered_flat_map.hpp>

namespace Simulation {
    typedef uint64_t u64;
    typedef u64 BlockID;
    using boost::unordered_flat_map;

    struct BlockDesc {};
    inline unordered_flat_map<BlockID, BlockDesc> blockRegistry;
}