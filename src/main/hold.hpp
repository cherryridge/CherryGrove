#pragma once
#include <atomic>
#include <function2/function2.hpp>
#include <SDL3/SDL.h>

#include "../globalState.hpp"
#include "../util/concurrentQueue.hpp"
#include "shutdown.hpp"

namespace Main {
    using std::memory_order_release, std::memory_order_acquire, fu2::function_view, Util::MPSCQueue;

    inline MPSCQueue<function_view<void()>> runOnMainThread;

    //threaded: Main loop.
    //Set `isCGAlive` to `false` to exit the program unconditionally.
    inline void hold() noexcept {
        SDL_Event event;
        while (isCGAlive.load(memory_order_acquire)) {
        //Populate new input frame
            const auto thisFrame = nextFrame_M.fetch_add(1, memory_order_release);
            FramedSDLEvents frame{1, {}};
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(windowHandle))) {
                    isCGAlive.store(false, memory_order_release);
                    goto stop;
                }
                frame.events.push_back(event);
            }
            eventQueue_M2R.enqueue(frame);
            if (Simulation::gameStarted.load(memory_order_acquire)) eventQueue_M2S.enqueue(frame);
        //end
        //Process Simulation stop signal
            //todo: Make a better one.
            if (Simulation::gameStopSignal.load(memory_order_acquire)) {
                Simulation::exit();
                Simulation::gameStopSignal.store(false, memory_order_release);
            }
        //end
        //Drain `runOnMainThread` MPSCQueue
            //todo: set a maximum number of tasks per loop?
            function_view<void()> task;
            while (runOnMainThread.dequeue(task)) task();
        //end
        }
        stop: shutdown();
    }
}