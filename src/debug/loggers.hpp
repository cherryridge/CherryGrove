#pragma once
#include <string>

#include "AtomicizedOutput.hpp"
#include "Logger.hpp"
#include "threadLocals.hpp"

namespace Debug {
    using std::string;

    inline Logger lout(&atomicCout, ""), lerr(&atomicCerr, "Error");

    inline void setThreadName(const string& name) noexcept {
        detail::threadName = name;
    }
}

using Debug::detail::nlaf, Debug::detail::newLineOnly, Debug::detail::flushOnly, Debug::lout, Debug::lerr;