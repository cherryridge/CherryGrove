#pragma once
#include <atomic>
#include <thread>

#include "../globalState.hpp"
#include "../input/InputHandler.hpp"
#include "../util/time.hpp"
#include "runOnSimThread.hpp"
#include "states.hpp"
#include "temporary.hpp"
#include "tick.hpp"

namespace Simulation::internal {
    using std::memory_order_acquire, std::memory_order_release, std::this_thread::sleep_for;

    inline void processSignal() noexcept {
        switch (signal.load(memory_order_acquire)) {
            using enum SimSignal;
            case None: break;
            case Start:
                if (!isSimStarted()) {
                    //todo: start the sim.
                    TemporaryCode::startGame();
                    simStarted.store(true, memory_order_release);
                    simPaused.store(false, memory_order_release);
                }
                break;
            case Pause:
                if (isSimStarted() && !isSimPaused()) {
                    //todo: pause the sim.
                    simPaused.store(true, memory_order_release);
                }
                break;
            case Resume:
                if (isSimStarted() && isSimPaused()) {
                    //todo: resume the sim.
                    simPaused.store(false, memory_order_release);
                }
                break;
            case Exit:
                if (isSimStarted()) {
                    //todo: exit the sim.
                    simStarted.store(false, memory_order_release);
                    simPaused.store(false, memory_order_release);
                    TemporaryCode::exitGame();
                }
                break;
        }
        signal.store(SimSignal::None, memory_order_release);
    }

    inline void hold() noexcept {
        while (GlobalState::isCGAlive()) {
            const auto startTime = Util::now();

            processSignal();

            if (Simulation::isSimStarted() && !Simulation::isSimPaused()) tick();

            InputHandler::processTrigger();
            InputHandler::processPersist();

            internal::processTasks();

            const auto elapsedTime = Util::timeDiffUs(startTime, Util::now());
            if (elapsedTime < 20000) sleep_for(Util::asUs(20000 - elapsedTime));
        }
    }
}