#pragma once
#include <atomic>
#include <bgfx/bgfx.h>
#include <function2/function2.hpp>
#include <SDL3/SDL.h>

#include "../boot/focus/Focus.hpp"
#include "../globalState.hpp"
#include "../input/inputPipeline.hpp"
#include "../settings/Settings.hpp"
#include "../util/concurrentQueue.hpp"
#include "../util/time.hpp"
#include "../window.hpp"
#include "shutdown.hpp"

namespace Main {
    typedef uint64_t u64;
    using std::memory_order_relaxed, std::memory_order_acquire, fu2::function_view, Util::MPSCQueue, InputHandler::MAXIMUM_INPUT_EVENTS_PER_FRAME;

    inline MPSCQueue<function_view<void()>> runOnMainThread;

    //threaded: Main loop.
    inline void hold() noexcept {
        const u64
            maxTasks = Settings::getSettings().debug.maxMainThreadTasksPerFrame,
            maxTaskTimeUs = Settings::getSettings().debug.maxMainThreadTaskTimeUs,
            maxRenderWaitTimeMs = Settings::getSettings().debug.maxMainThreadRenderWaitTimeMs;

        Util::TimePoint taskStartTime;
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
                InputHandler::inputQueue_M2S.enqueue(frame);
            }

        //Render bgfx frame
            bgfx::renderFrame(maxRenderWaitTimeMs);
    
        //Drain `runOnMainThread` MPSCQueue
            taskStartTime = Util::now();
            for (u64 i = 0; i < maxTasks && Util::timeDiffUs(taskStartTime, Util::now()) < maxTaskTimeUs; i++) if (runOnMainThread.dequeue(task)) task();
        }
        stop: shutdown();
    }
}