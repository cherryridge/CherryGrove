#pragma once
#include <function2/function2.hpp>

#include "../util/concurrentQueue.hpp"
#include "../util/time.hpp"

namespace Main {
    typedef uint64_t u64;
    using fu2::function_view, Util::MPSCQueue;

    namespace detail {
        inline MPSCQueue<function_view<void()>> queue;
        inline function_view<void()> task;
        inline u64 maxTasks, maxTaskTimeUs;
        inline Util::TimePoint taskStartTime;
    }

    inline void runOnMainThread(function_view<void()> task) noexcept {
        detail::queue.enqueue(task);
    }

    namespace internal {
        inline void setTaskProcessLimits(u64 maxTasks, u64 maxTaskTimeUs) noexcept {
            detail::maxTasks = maxTasks;
            detail::maxTaskTimeUs = maxTaskTimeUs;
        }

        inline void processTasks() noexcept {
            detail::taskStartTime = Util::now();
            for (u64 i = 0; i < detail::maxTasks && Util::timeDiffUs(detail::taskStartTime, Util::now()) < detail::maxTaskTimeUs; i++) if (detail::queue.dequeue(detail::task)) detail::task();
        }
    }
}