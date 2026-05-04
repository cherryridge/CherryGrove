#pragma once
#include <atomic>

namespace Sound {
    typedef uint64_t u64;
    using std::atomic;

    inline atomic<u64> perf_audioUSPT{0};
}