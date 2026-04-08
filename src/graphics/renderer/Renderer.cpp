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
#include "../gui/Gui.hpp"
#include "../shader/ShaderPool.hpp"
#include "../texture/TexturePool.hpp"
#include "Renderer.hpp"

namespace Renderer {
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::array, std::thread, std::atomic, std::this_thread::yield, std::memory_order_acquire, std::memory_order_release,  Simulation::playerEntity, bgfx::VertexBufferHandle, bgfx::VertexLayout, bgfx::IndexBufferHandle, bgfx::Init, bgfx::PlatformData, bgfx::Attrib, bgfx::AttribType, bgfx::createVertexBuffer, bgfx::createIndexBuffer, bgfx::makeRef;
    static void renderLoop() noexcept;

    atomic<bool> initialized {false};
    WindowInfoCache cache;

    static VertexBufferHandle vertexBuffer;
    static IndexBufferHandle indexBuffer;
    static thread rendererThread;

    void init() noexcept {
        rendererThread = thread(&renderLoop);
        //Wait for bgfx to be initialized.
        for (u64 i = 0; !initialized.load(memory_order_acquire); i++) if (i & 0xFFull) yield();
    }
    void shutdown() noexcept { rendererThread.join(); }

    static void bgfx_SP_TP_init() noexcept {
        Init config;
        PlatformData pdata;
        auto propertyHandle = SDL_GetWindowProperties(GlobalState::windowHandle);
        #if CG_PLATFORM_WINDOWS
            pdata.ndt = nullptr;
            pdata.nwh = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        #elif CG_PLATFORM_LINUX
            const char* sessionType = secure_getenv("XDG_SESSION_TYPE");
            if ((sessionType && strcmp(sessionType, "wayland") == 0) || getenv("WAYLAND_DISPLAY")) {
                pdata.ndt = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
                pdata.nwh = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_WAYLAND_EGL_WINDOW_POINTER, nullptr);
                pdata.type = bgfx::NativeWindowHandleType::Wayland;
            }
            else{
                pdata.ndt = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
                pdata.nwh = reinterpret_cast<void*>(SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, nullptr));
                pdata.type = bgfx::NativeWindowHandleType::Default;
            }
        #elif CG_PLATFORM_MACOS
            pdata.ndt = nullptr;
            pdata.nwh = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
        #elif CG_PLATFORM_ANDROID
            Fatal::exit(Fatal::MISC_UNSUPPORTED_PLATFORM);
        #elif CG_PLATFORM_IOS
            Fatal::exit(Fatal::MISC_UNSUPPORTED_PLATFORM);
        #endif
        //Let bgfx select the rendering backend automatically.
        config.type = bgfx::RendererType::Count;
        //Let bgfx select the adapter automatically.
        config.vendorId = BGFX_PCI_ID_NONE;
        i32 width, height;
        SDL_GetWindowSize(GlobalState::windowHandle, &width, &height);
        config.resolution.width = width;
        config.resolution.height = height;
        config.resolution.reset = BGFX_RESET_VSYNC;
        config.platformData = pdata;
        if (!bgfx::init(config)) {
            lerr << "[Renderer] Failed to initialize bgfx!" << endl;
            Fatal::exit(Fatal::BGFX_INITIALIZATION_FAILED);
        }
        lout << "Using rendering backend: " << bgfx::getRendererName(bgfx::getRendererType()) << endl;

    //Initialize pools
        ShaderPool::init();
        TexturePool::init();

    //Initialize vertex layout
        VertexLayout layout;
        layout.begin().add(Attrib::Position, 3, AttribType::Float, true).add(Attrib::TexCoord0, 2, AttribType::Float, true).end();
        vertexBuffer = createVertexBuffer(makeRef(&blockVerticesTemplate, sizeof(blockVerticesTemplate)), layout);
        indexBuffer = createIndexBuffer(makeRef(&blockIndicesTemplate, sizeof(blockIndicesTemplate)));
    }

    static void renderLoop() noexcept {
        using namespace Components;
        lout << "Renderer" << flush;
        lout << "Hello from renderer thread!" << endl;
        bgfx_SP_TP_init();
        Gui::init();
        initialized.store(true, memory_order_release);
        SDL_GetWindowSize(GlobalState::windowHandle, &cache.width, &cache.height);
        cache.aspectRatio = SDL_GetWindowDisplayScale(GlobalState::windowHandle);
        bgfx::reset(cache.width, cache.height, BGFX_RESET_VSYNC);
        while (GlobalState::isCGAlive) {
        //Prepare for rendering
            //Refresh windowHandle size
            //if (sizeUpdateSignal) {
            //    SDL_GetWindowSize(GlobalState::windowHandle, &cache.width, &cache.height);
            //    cache.aspectRatio = SDL_GetWindowDisplayScale(GlobalState::windowHandle);
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

                bgfx::submit()
            }
            else bgfx::touch(gameViewId);
        //end
            bgfx::frame();
        //Process input events
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
