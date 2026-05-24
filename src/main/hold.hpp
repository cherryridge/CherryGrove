#pragma once
#include <atomic>
#include <bgfx/bgfx.h>
#include <SDL3/SDL.h>

#include "../boot/focus/Focus.hpp"
#include "../globalState.hpp"
#include "../input/inputPipeline.hpp"
#include "../settings/Settings.hpp"
#include "../window.hpp"
#include "runOnMainThread.hpp"
#include "shutdown.hpp"

namespace Main {
    typedef uint64_t u64;
    using std::memory_order_relaxed, std::memory_order_acquire, InputHandler::MAXIMUM_INPUT_EVENTS_PER_FRAME;

    //threaded: Main loop.
    inline void hold() noexcept {
        const auto& debug = Settings::getSettings().debug;
        const u64 maxRenderWaitTimeMs = debug.maxMainThreadRenderWaitTimeMs;
        internal::setTaskProcessLimits(debug.maxMainThreadTasksPerFrame, debug.maxMainThreadTaskTimeUs);

        SDL_Event event;
        InputHandler::FramedSDLEvents frame;

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
    
        //Process main thread tasks
            internal::processTasks();
        }
        stop: shutdown();
    }
}