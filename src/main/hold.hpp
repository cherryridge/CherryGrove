#pragma once
#include <atomic>
#include <chrono>
#include <bgfx/bgfx.h>
#include <function2/function2.hpp>
#include <SDL3/SDL.h>

#include "../boot/focus/Focus.hpp"
#include "../globalState.hpp"
#include "../input/inputPipeline.hpp"
#include "../settings/Settings.hpp" // IWYU pragma: keep
#include "../util/concurrentQueue.hpp"
#include "../window.hpp"
#include "shutdown.hpp"

namespace Main {
    typedef uint64_t u64;
    using std::memory_order_relaxed, std::memory_order_acquire, std::chrono::high_resolution_clock, std::chrono::microseconds, std::chrono::milliseconds, std::chrono::time_point, std::chrono::duration_cast, fu2::function_view, Util::MPSCQueue, InputHandler::MAXIMUM_INPUT_EVENTS_PER_FRAME;

    inline MPSCQueue<function_view<void()>> runOnMainThread;

    //threaded: Main loop.
    inline void hold() noexcept {
        const u64 maxTasks = Settings::getSettings().debug.maxMainThreadTasksPerFrame;
        const microseconds maxTaskTime = microseconds(Settings::getSettings().debug.maxMainThreadTaskTimeUs);
        const milliseconds maxRenderWaitTime = milliseconds(Settings::getSettings().debug.maxMainThreadRenderWaitTimeMs);

        time_point<high_resolution_clock> taskStartTime;
        SDL_Event event;
        InputHandler::FramedSDLEvents frame;
        function_view<void()> task;

        while (GlobalState::isCGAlive()) {
        //Check for focus messages
            Boot::Focus::tryReceive();

        //Populate new input frame
            for (frame.actualSize = 0; frame.actualSize < MAXIMUM_INPUT_EVENTS_PER_FRAME; frame.actualSize++) {
                if (SDL_PollEvent(&event)) {
                    if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(Window::getMainWindow()))) {
                        GlobalState::setIsCGAlive(false);
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

        //Render bgfx frame.
            bgfx::renderFrame(maxRenderWaitTime.count());
    
        //Drain `runOnMainThread` MPSCQueue
            taskStartTime = high_resolution_clock::now();
            for (u64 i = 0; i < maxTasks && high_resolution_clock::now() - taskStartTime < maxTaskTime; i++) if (runOnMainThread.dequeue(task)) task();
        #if CG_DEBUG
            if (high_resolution_clock::now() - taskStartTime > maxTaskTime) lout << "Can't keep up! Main thread task list run for " << duration_cast<microseconds>(high_resolution_clock::now() - taskStartTime).count() << " / " << duration_cast<microseconds>(maxTaskTime).count() << "us" << endl;
        #endif
        }
        stop: shutdown();
    }
}