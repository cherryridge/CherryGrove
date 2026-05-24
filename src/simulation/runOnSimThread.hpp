#pragma once
#include <function2/function2.hpp>

#include "../util/concurrentQueue.hpp"

namespace Simulation {
    typedef uint64_t u64;
    using fu2::function_view, Util::MPSCQueue;

    namespace detail {
        inline MPSCQueue<function_view<void()>> queue;
        inline function_view<void()> task;
    }

    inline void runOnSimThread(function_view<void()> task) noexcept {
        detail::queue.enqueue(task);
    }

    namespace internal {
        inline void processTasks() noexcept {
            while (detail::queue.dequeue(detail::task)) detail::task();
        }
    }
}