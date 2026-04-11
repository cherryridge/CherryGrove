#pragma once
#include <array>
#include <atomic>
#include <thread>
#include <backends/imgui_impl_sdl3.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <SDL3/SDL.h>

#include "../../components/Components.hpp"
#include "../../debug/Fatal.hpp"
#include "../../debug/Logger.hpp"
#include "../../globalState.hpp"
#include "../../input/inputPipeline.hpp"
#include "../../simulation/Simulation.hpp"
#include "../../util/os/platform.hpp"
#include "../../window.hpp"
#include "../backend/imgui_impl_bgfx.hpp"
#include "../controller.hpp"
#include "../gui/Gui.hpp"
#include "../shader/ShaderPool.hpp"
#include "../texture/TexturePool.hpp"


namespace Renderer {
    typedef uint8_t u8;
    typedef int32_t i32;
    using std::atomic, std::array;

    extern atomic<bool> initialized;
    
    //Must be called after `Window::init`.
    void init() noexcept;
    void shutdown() noexcept;

    inline constexpr u8 guiViewId = 1u, gameViewId = 0u;

    struct Vertex {
        float x, y, z;
        //Texture coordinates are normalized from `i16` during conversion.
        float u, v;
    };
}

namespace Graphics::Renderer {
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::array, std::thread, std::atomic, std::this_thread::yield, std::memory_order_acquire, std::memory_order_release, Simulation::playerEntity;
    static void renderLoop() noexcept;

    inline atomic<bool> initialized{false};

    struct WindowInfoCache {
        i32 width, height;
        float aspectRatio;
    };

    inline WindowInfoCache cache;

    static bgfx::VertexBufferHandle vertexBuffer;
    static bgfx::IndexBufferHandle indexBuffer;
    static thread rendererThread;

    void init() noexcept {
        rendererThread = thread(renderLoop);
        //Wait for bgfx to be initialized.
        for (u64 i = 0; !initialized.load(memory_order_acquire); i++) if (i & 0xFFull) yield();
    }
    void shutdown() noexcept { rendererThread.join(); }

    static void bgfx_SP_TP_init() noexcept {
        

    //Initialize pools
        ShaderPool::init();
        TexturePool::init();

    //Initialize vertex layout
        bgfx::VertexLayout layout;
        layout.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float, true).add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true).end();
        vertexBuffer = createVertexBuffer(bgfx::makeRef(&blockVerticesTemplate, sizeof(blockVerticesTemplate)), layout);
        indexBuffer = createIndexBuffer(bgfx::makeRef(&blockIndicesTemplate, sizeof(blockIndicesTemplate)));
    }

    static void renderLoop() noexcept {
        using namespace Components;
        lout << "Renderer" << flush;
        lout << "Hello from renderer thread!" << endl;
        bgfx_SP_TP_init();
        Gui::init();
        initialized.store(true, memory_order_release);
        SDL_GetWindowSize(Window::getMainWindow(), &cache.width, &cache.height);
        cache.aspectRatio = SDL_GetWindowDisplayScale(Window::getMainWindow());
        bgfx::reset(cache.width, cache.height, BGFX_RESET_VSYNC);
        while (GlobalState::isCGAlive.load(memory_order_acquire)) {
        //Prepare for rendering
            //Refresh windowHandle size
            //if (sizeUpdateSignal) {
            //    SDL_GetWindowSize(Window::getMainWindow(), &cache.width, &cache.height);
            //    cache.aspectRatio = SDL_GetWindowDisplayScale(Window::getMainWindow());
            //    bgfx::reset(cache.width, cache.height);
            //    sizeUpdateSignal = false;
            //}
        #if DEBUG
            bgfx::setDebug(BGFX_DEBUG_STATS | BGFX_DEBUG_PROFILER | BGFX_DEBUG_TEXT);
        #endif
        //Render GUI
            bgfx::setViewClear(guiViewId, BGFX_CLEAR_NONE, 0x00000000);
            bgfx::setViewRect(guiViewId, 0, 0, cache.width, cache.height);
            Gui::render();
        //end
        //Render game content
            bgfx::setViewClear(gameViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x5bd093ff);
            bgfx::setViewRect(gameViewId, 0, 0, cache.width, cache.height);
            if (Simulation::gameStarted.load(memory_order_acquire)) {
            //Prepare render environment
                array<float, 16> view{}, proj{};
                Rotation::getViewMtx(view.data(), playerEntity);
                Camera::getProjMtx(proj.data(), playerEntity, cache.aspectRatio);
                bgfx::setViewTransform(gameViewId, view.data(), proj.data());
            //Render blocks from chunk meshes
                //todo:
            //Render entities

                //bgfx::submit();
            }
            else bgfx::touch(gameViewId);
        //end
            bgfx::frame();
        //Process input events
            //todo: I think this part is outdated. Review this.
            const u64 nextFrame = InputHandler::nextFrame_M.load(memory_order_acquire);
            while(true) {
                const auto* ptr = InputHandler::inputQueue_M2R.peek();
                //We need to keep up with main thread
                if (ptr && ptr->frame < nextFrame) {
                    InputHandler::FramedSDLEvents framedEvents;
                    //Discard the success flag because we already peeked.
                    //Don't use `ptr` directly.
                    static_cast<void>(InputHandler::inputQueue_M2R.dequeue(framedEvents));
                    if (Simulation::gameStarted.load(memory_order_acquire)) {
                        auto& io = ImGui::GetIO();
                        InputHandler::flagQueue_R2S.enqueue({framedEvents.frame, io.WantCaptureMouse, io.WantCaptureKeyboard, io.WantTextInput, io.WantSetMousePos});
                    }
                    for (u64 i = 0; i < framedEvents.events.size(); i++) ImGui_ImplSDL3_ProcessEvent(&framedEvents.events[i]);
                }
                else break;
            }
        //end
        }
        lout << "Terminating renderer thread!" << endl;
        initialized.store(false, memory_order_release);
        Gui::shutdown();
        TexturePool::shutdown();
        ShaderPool::shutdown();
        bgfx::shutdown();
    }
}
