#pragma once
#include <atomic>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <type_traits>

#include "../util/os/filesystem.hpp"
#include "../util/os/thread.hpp"
#include "AtomicizedOutput.hpp"
#include "manips.hpp"
#include "threadLocals.hpp"

namespace Debug {
    typedef uint64_t u64;
    using std::cout, std::cerr, std::endl, std::format, std::is_function_v, std::memory_order_relaxed, std::forward, std::string, std::filesystem::path, Util::OS::getU8String;

    //We've rewritten the Logger to allow arbitrary amount of Logger instances, but now every logger instance needs to have static lifetime duration to avoid vector index invalidation.
    struct Logger {
    private:
        AtomicizedOutput* output;
        const string loggerName;
        const u64 id;

        void ensureBufferExists() noexcept {
            if (detail::buffers.size() <= id) detail::buffers.resize(id + 1);
        }

    public:
        explicit Logger(AtomicizedOutput* output, const string& loggerName = "") noexcept : output(output), loggerName(loggerName), id(detail::nextLoggerInstanceId.fetch_add(1, memory_order_relaxed)) {}

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        //... So we won't get silently fucked if we ALWAYS accidentally forgot to wrap them in `Util::OS::getU8String()`.
        Logger& operator<(const path& filePath) noexcept {
            *this << getU8String(filePath);
            return *this;
        }

        Logger& operator<<(detail::Manip manip) noexcept {
            ensureBufferExists();
            if (output == nullptr) return *this;
            if (manip == detail::nlaf || manip == detail::newLineOnly) detail::buffers[id] << "\n";
            if ((manip == detail::nlaf || manip == detail::flushOnly) && !detail::buffers[id].str().empty()) {
                string temp;
                if (!loggerName.empty()) temp += format("({}) ", loggerName);
                if (!detail::threadName.empty()) temp += format("[{}] ", detail::threadName);
                else temp += format("[{}] ", Util::getThreadId());
                temp += detail::buffers[id].str();
                detail::buffers[id].str({});
                detail::buffers[id].clear();
                *output << temp;
                output->flush();
            }
        #if CG_DEBUG
            if (manip != detail::nlaf && manip != detail::newLineOnly && manip != detail::flushOnly) *this << "[Logger] Warning: Unrecognized manipulator function! Check `manips.hpp`." << detail::nlaf;
        #endif
            return *this;
        }

        template <typename T>
        Logger& operator<<(T&& value) noexcept {
            ensureBufferExists();
            if (output == nullptr) return *this;
            detail::buffers[id] << value;
            return *this;
        }

        //This doesn't flush anything. Everything is still buffered until you use `nlaf` or `flushOnly`.
        void redirectOutput(AtomicizedOutput* newOutput) noexcept { output = newOutput; }

        Logger& operator()(auto&&... values) noexcept {
            ensureBufferExists();
            if (output == nullptr) return *this;
            ((*this << forward<decltype(values)>(values)), ...);
            *this << detail::nlaf;
            return *this;
        }
    };
}