#include <atomic>
#include <thread>
#include <backends/imgui_impl_sdl3.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <SDL3/SDL.h>

#include "../components/Components.hpp"
#include "../debug/Fatal.hpp"
#include "../debug/Logger.hpp"
#include "../gui/Gui.hpp"
#include "../input/eventPipeline.hpp"
#include "../Main.hpp"
#include "../simulation/Simulation.hpp"
#include "../util/platform.hpp"
#include "Renderer.hpp"
#include "ShaderPool.hpp"
#include "TexturePool.hpp"

namespace Renderer {
    typedef int32_t i32;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::thread, std::atomic, std::this_thread::yield, std::memory_order_acquire, std::memory_order_release, Simulation::gameRegistry, Simulation::playerEntity, bgfx::VertexBufferHandle, bgfx::VertexLayout, bgfx::IndexBufferHandle, bgfx::Init, bgfx::PlatformData, bgfx::Attrib, bgfx::AttribType, bgfx::createVertexBuffer, bgfx::createIndexBuffer, bgfx::makeRef;
    static void renderLoop() noexcept;

    atomic<bool> initialized {false};//, sizeUpdateSignal {true};
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
        auto propertyHandle = SDL_GetWindowProperties(Main::windowHandle);
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
        //Let bgfx auto select rendering backend.
        config.type = bgfx::RendererType::Count;
        //Control for switching backend temporaily for debug.
        //config.type = bgfx::RendererType::Vulkan;
        //Let bgfx auto select adapter.
        config.vendorId = BGFX_PCI_ID_NONE;
        i32 width, height;
        SDL_GetWindowSize(Main::windowHandle, &width, &height);
        config.resolution.width = width;
        config.resolution.height = height;
        config.resolution.reset = BGFX_RESET_VSYNC;
        config.platformData = pdata;
        if (!bgfx::init(config)) {
            lerr << "[Renderer] Failed to initialize bgfx!" << endl;
            Fatal::exit(Fatal::BGFX_INITIALIZATION_FALILED);
        }
        lout << "Using rendering backend: " << bgfx::getRendererName(bgfx::getRendererType()) << endl;

    //Initialize pools
        ShaderPool::init();
        TexturePool::init("s_texture");

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
        SDL_GetWindowSize(Main::windowHandle, &cache.width, &cache.height);
        cache.aspectRatio = SDL_GetWindowDisplayScale(Main::windowHandle);
        bgfx::reset(cache.width, cache.height, BGFX_RESET_VSYNC);
        while (Main::isCGAlive) {
        //Prepare for rendering
            //Refresh windowHandle size
            //if (sizeUpdateSignal) {
            //    SDL_GetWindowSize(Window::windowHandle, &cache.width, &cache.height);
            //    cache.aspectRatio = SDL_GetWindowDisplayScale(Window::windowHandle);
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
                float view[16]{}, proj[16]{};
                Rotation::getViewMtx(view, playerEntity);
                Camera::getProjMtx(proj, playerEntity, cache.aspectRatio);
                bgfx::setViewTransform(gameViewId, view, proj);
            //Render opaque parts/blocks
                //Get all renderable blocks
                auto group = gameRegistry.group<const BlockCoordinatesComp, const BlockRenderComp>();
                group.each([](entt::entity entity, const BlockCoordinatesComp& coords, const BlockRenderComp& renderData) {
                    float worldSpaceTranslate[16]{};
                    bx::mtxTranslate(worldSpaceTranslate, (float)coords.x, (float)coords.y, (float)coords.z);
                    for (const auto& [cubeIndex, cube] : renderData.subcubes) {
                        float subcubeTransform[16]{};
                        //Subcube translate
                        bx::mtxTranslate(subcubeTransform, cube.origin.x, cube.origin.y, cube.origin.z);
                        //todo: Subcube rotation
                        //bx::mtxRotateXYZ(transform, );
                        for (i32 i = 0; i < 6; i++) if (cube.faces[i].shaderId) {
                            float transform[16]{};
                            bx::mtxMul(transform, worldSpaceTranslate, subcubeTransform);
                            bgfx::setTransform(transform);
                            bgfx::setVertexBuffer(0, vertexBuffer, i * 4, 4);
                            bgfx::setIndexBuffer(indexBuffer);
                            TexturePool::useTexture(cube.faces[i].textureId);
                            bgfx::submit(gameViewId, ShaderPool::getShader(cube.faces[i].shaderId));
                        }
                    }
                });
            //Render translucent blocks
                //todo
            }
            else bgfx::touch(gameViewId);
        //end
            bgfx::frame();
        //Process input events
            const u64 nextFrame = InputHandler::nextFrame_M.load(memory_order_acquire);
            while(true) {
                const auto* ptr = InputHandler::eventQueue_M2R.peek();
                //We need to keep up with main thread
                if (ptr && ptr->frame < nextFrame) {
                    InputHandler::FramedSDLEvents framedEvents;
                    //Discard the success flag because we already peeked.
                    //Don't use `ptr` directly.
                    static_cast<void>(InputHandler::eventQueue_M2R.dequeue(framedEvents));
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
