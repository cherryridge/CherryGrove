#pragma once
#include <atomic>
#include <thread>

#include "../debug/loggers.hpp"
#include "../globalState.hpp"
#include "../input/InputHandler.hpp"
#include "../util/time.hpp"
#include "tick.hpp"
#include "states.hpp"

namespace Simulation::internal {
    using std::memory_order_acquire, std::memory_order_release, std::this_thread::sleep_for;

    inline void processSignal() noexcept {
        using namespace Simulation::detail;
        switch (signal.load(memory_order_acquire)) {
            using enum SimSignal;
            case None: break;
            case Start:
                if (!isSimStarted()) {
                    //todo: start the sim.
                }
                break;
            case Pause:
                if (isSimStarted() && !isSimPaused()) {
                    //todo: pause the sim.
                }
                break;
            case Resume:
                if (isSimStarted() && isSimPaused()) {
                    //todo: resume the sim.
                }
                break;
            case Exit:
                if (isSimStarted()) {
                    //todo: exit the sim.
                }
                break;
        }
        signal.store(SimSignal::None, memory_order_release);
    }

    inline void hold() noexcept {
        Debug::setThreadName("Simulation");
        lout << "Hello from simulation thread!" << nlaf;
        while (GlobalState::isCGAlive()) {
            processSignal();
            if (Simulation::isSimStarted() && !Simulation::isSimPaused()) {
                const auto startTime = Util::now();
                tick();
                const auto endTime = Util::now();
                const auto elapsedTime = Util::timeDiffUs(startTime, endTime);
                if (elapsedTime < 20000) sleep_for(Util::asUs(20000 - elapsedTime));
            }
            InputHandler::processTrigger();
            InputHandler::processPersist();
        }
        lout << "Simulation thread terminated!" << nlaf;
    }
}