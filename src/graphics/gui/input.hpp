#pragma once
#include <atomic>
#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>

#include "../../input/inputPipeline.hpp"
#include "../../simulation/Simulation.hpp"

namespace Gui {
    typedef uint64_t u64;
    using std::atomic, std::memory_order_acquire;

    inline void handleInput() noexcept {
        //fixme: I think this part is outdated. Review this.
        const u64 nextFrame = InputHandler::nextFrame_M.load(memory_order_acquire);
        while(true) {
            const auto* ptr = InputHandler::inputQueue_M2R.peek();
            //We need to keep up with main thread
            if (ptr && ptr->frame < nextFrame) {
                InputHandler::FramedSDLEvents framedEvents;
                //Discard the success flag because we've already peeked.
                static_cast<void>(InputHandler::inputQueue_M2R.dequeue(framedEvents));
                if (Simulation::gameStarted.load(memory_order_acquire)) {
                    //This is not possible to deloop this variable because it's volatile relative to us and reference accessing is almost always optimized, so it's whatever.
                    const auto& io = ImGui::GetIO();
                    InputHandler::flagQueue_R2S.enqueue({framedEvents.frame, io.WantCaptureMouse, io.WantCaptureKeyboard, io.WantTextInput, io.WantSetMousePos});
                }
                for (u64 i = 0; i < framedEvents.events.size(); i++) ImGui_ImplSDL3_ProcessEvent(&framedEvents.events[i]);
            }
            else break;
        }
    }
}