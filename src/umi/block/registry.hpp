#pragma once
#include <atomic>
#include <vector>

#include "BlockDefinition.hpp"

namespace Umi::Block {
    typedef uint64_t u64;
    using std::atomic, std::vector;

    namespace detail {
        inline atomic<u64> nextBlockId{1ull};
        inline vector<BlockDefinition> blockDefinitions;
    }

    [[nodiscard]] inline u64 registerBlock() noexcept {

    }
}