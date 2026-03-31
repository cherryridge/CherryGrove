#pragma once
#include <atomic>
#include <memory>
#include <vector>
#include <SDL3/SDL.h>

#include "../util/concurrentQueue.hpp"

namespace InputHandler {
    typedef uint64_t u64;
    using std::atomic, std::shared_ptr, std::vector, Util::SPSCQueue;

    struct FramedSDLEvents {
        u64 frame;
        vector<SDL_Event> events;
    };

    struct FramedImGuiFlags {
        u64 frame;
        bool wantCaptureMouse{false}, wantCaptureKeyboard{false}, wantTextInput{false}, wantSetMousePos{false};
    };

    inline atomic<shared_ptr<const FramedSDLEvents>> current{shared_ptr<const FramedSDLEvents>(new FramedSDLEvents{0, {}})};
    inline FramedSDLEvents buffer_M{0, {}};
    inline SPSCQueue<FramedImGuiFlags> flagQueue_R2S;
    inline SPSCQueue<FramedSDLEvents> eventQueue_M2R, eventQueue_M2S;
    inline atomic<u64> nextFrame_M{0};
}