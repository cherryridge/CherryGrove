#pragma once

#include "../util/concurrentQueue.hpp"
#include "command.hpp"

namespace Sound::detail {
    using Util::MPSCQueue;

    inline MPSCQueue<Command> commandQueue;
}