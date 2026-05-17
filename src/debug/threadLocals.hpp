#pragma once
#include <atomic>
#include <sstream>
#include <string>
#include <vector>

namespace Debug::detail {
    typedef uint64_t u64;
    using std::atomic, std::ostringstream, std::string, std::vector;

    inline thread_local string threadName;
    inline atomic<u64> nextLoggerInstanceId{0};
    inline thread_local vector<ostringstream> buffers;
}