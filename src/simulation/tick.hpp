#pragma once
#include <thread>

#include "../util/time.hpp"

namespace Simulation::internal {
    using std::this_thread::sleep_for;

    inline void tick() noexcept {
        sleep_for(Util::asMs(10));
    //1. Update global state and wait for Renderer to finish (if it's not finished already)

    //2. Send start signal to all region threads

    //3. Wait for all region threads to finish

    //4. Invoke Renderer to render the state

    //5. Update loading zones

    //6. Save to disk
    }
}