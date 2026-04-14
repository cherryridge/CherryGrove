#pragma once
#include <atomic>
#include <chrono>
#include <function2/function2.hpp>
#include <SDL3/SDL.h>

#include "../globalState.hpp"
#include "../input/inputPipeline.hpp"
#include "../settings/Settings.hpp"
#include "../util/concurrentQueue.hpp"
#include "../window.hpp"
#include "shutdown.hpp"

namespace Main {
    typedef uint64_t u64;
    using std::memory_order_relaxed, std::memory_order_release, std::memory_order_acquire, std::chrono::high_resolution_clock, std::chrono::microseconds, std::chrono::duration_cast, fu2::function_view, Util::MPSCQueue, InputHandler::MAXIMUM_INPUT_EVENTS_PER_FRAME;

    inline MPSCQueue<function_view<void()>> runOnMainThread;

    //threaded: Main loop.
    inline void hold() noexcept {
        const u64 maximumMainThreadFunctionsPerFrame = Settings::getSettings().debug.maximumMainThreadFunctionsPerFrame;
        const auto maximumMainThreadLoopTimeUs = microseconds(Settings::getSettings().debug.maximumMainThreadLoopTimeUs);
        auto loopStartTime = high_resolution_clock::now();

        SDL_Event event;
        InputHandler::FramedSDLEvents frame{};
        function_view<void()> task;

        while (GlobalState::isCGAlive.load(memory_order_acquire)) {
            loopStartTime = high_resolution_clock::now();

        //Populate new input frame
            for (frame.actualSize = 0; frame.actualSize < MAXIMUM_INPUT_EVENTS_PER_FRAME; frame.actualSize++) {
                if (SDL_PollEvent(&event)) {
                    if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(Window::getMainWindow()))) {
                        GlobalState::isCGAlive.store(false, memory_order_release);
                        goto stop;
                    }
                    frame.events[frame.actualSize] = event;
                }
                else break;
            }
            if (frame.actualSize != 0) {
                frame.frameId = InputHandler::nextFrameId_M.fetch_add(1, memory_order_relaxed);
                InputHandler::inputQueue_M2R.enqueue(frame);
                if (Simulation::gameStarted.load(memory_order_acquire)) InputHandler::inputQueue_M2S.enqueue(frame);
            }

        #if CG_DEBUG
            if (high_resolution_clock::now() - loopStartTime > maximumMainThreadLoopTimeUs) lout << "[Main] Can't keep up! Is CherryGrove Minecraft? Input section run for " << duration_cast<microseconds>(high_resolution_clock::now() - loopStartTime).count() << "us while set maximum loop time is " << duration_cast<microseconds>(maximumMainThreadLoopTimeUs).count() << "us" << endl;
        #endif

        //Drain `runOnMainThread` MPSCQueue
            for (u64 i = 0; i < maximumMainThreadFunctionsPerFrame && high_resolution_clock::now() - loopStartTime < maximumMainThreadLoopTimeUs; i++) if (runOnMainThread.dequeue(task)) task();
        }
        stop: shutdown();
    }
}
