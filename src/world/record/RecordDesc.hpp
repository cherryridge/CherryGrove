#pragma once
#include <string>

#include "../../util/RuntimeBijection.hpp"

namespace World::Record {
    typedef uint64_t u64;
    using std::string, Util::RuntimeBijection;

    struct RecordDesc {
        RuntimeBijection<CGIDHandle, u64> cgid_idx;

        //Make a unified name&space vs uuid resolver so we know which pack did what exactly. Shadowing is really complicated.
    };
}