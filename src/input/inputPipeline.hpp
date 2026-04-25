#pragma once
#include <array>
#include <atomic>
#include <memory>
#include <vector>
#include <SDL3/SDL.h>

#include "../util/concurrentQueue.hpp"
#include "InputHandler.hpp"

namespace InputHandler {
    typedef uint64_t u64;
    using std::array, std::atomic, std::shared_ptr, std::vector, Util::SPSCQueue;

    struct FramedSDLEvents {
        u64 frameId;
        //Why not use `u64`? `SDL_Event` is already 8-byte aligned.
        u64 actualSize{0};
        //note: This is changed from a vector so we have a heap allocation free event pipeline.
        array<SDL_Event, MAXIMUM_INPUT_EVENTS_PER_FRAME> events;
    };

    struct FramedImGuiFlags {
        u64 frame;
        bool wantCaptureMouse{false}, wantCaptureKeyboard{false}, wantTextInput{false}, wantSetMousePos{false};
    };

    inline SPSCQueue<FramedSDLEvents> inputQueue_M2R, inputQueue_M2S;
    inline SPSCQueue<FramedImGuiFlags> flagQueue_R2S;

    //threaded: Written by Main thread, read by Renderer and Simulation.
    inline atomic<u64> nextFrameId_M{0};
}