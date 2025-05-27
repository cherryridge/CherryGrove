#include <thread>
#include <atomic>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL3/SDL.h>

#include "../debug/Logger.hpp"
#include "../debug/Fatal.hpp"
#include "../components/Components.hpp"
#include "../CherryGrove.hpp"
#include "ShaderPool.hpp"
#include "TexturePool.hpp"
#include "../input/InputHandler.hpp"
#include "../gui/Gui.hpp"
#include "../gui/Window.hpp"
#include "../simulation/Simulation.hpp"
#include "Renderer.hpp"

namespace Renderer {
    typedef int32_t i32;
    typedef uint32_t u32;
    using std::thread, std::atomic, Simulation::gameRegistry, Simulation::playerEntity, bgfx::VertexBufferHandle, bgfx::VertexLayout, bgfx::IndexBufferHandle, bgfx::Init, bgfx::PlatformData, bgfx::Attrib, bgfx::AttribType, bgfx::createVertexBuffer, bgfx::createIndexBuffer, bgfx::makeRef;

    atomic<bool> initialized(false), sizeUpdateSignal(true);
    WindowInfoCache cache;

    VertexBufferHandle vertexBuffer;
    IndexBufferHandle indexBuffer;

    static void renderLoop() noexcept;
    thread rendererThread;

    void start() noexcept { rendererThread = thread(&renderLoop); }
    void shutdown() noexcept { rendererThread.join(); }

    static void initBGFX() noexcept {
        Init config;
        PlatformData pdata;
        auto propertyHandle = SDL_GetWindowProperties(Window::windowHandle);
        #if defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
            pdata.ndt = nullptr;
            pdata.nwh = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        #elif defined(__linux__)
            const char* sessionType = secure_getenv("XDG_SESSION_TYPE");
            if((sessionType && strcmp(sessionType, "wayland") == 0) || getenv("WAYLAND_DISPLAY")) {
                pdata.ndt = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
                pdata.nwh = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_WAYLAND_EGL_WINDOW_POINTER, nullptr);
                pdata.type = bgfx::NativeWindowHandleType::Wayland;
            }
            else{
                pdata.ndt = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
                pdata.nwh = reinterpret_cast<void*>(SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, nullptr));
                pdata.type = bgfx::NativeWindowHandleType::Default;
            }
        #elif defined(__ANDROID__)
            Fatal::exit(Fatal::MISC_UNSUPPORTED_PLATFORM);
        #elif defined(__APPLE__)
            #include <TargetConditionals.h>
            #if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
                Fatal::exit(Fatal::MISC_UNSUPPORTED_PLATFORM);
            #elif TARGET_OS_MAC
                pdata.ndt = nullptr;
                pdata.nwh = SDL_GetPointerProperty(propertyHandle, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
            #endif
        #endif
        //Let bgfx auto select rendering backend.
        config.type = bgfx::RendererType::Count;
        //Control for switching backend temporaily for debug.
        //config.type = bgfx::RendererType::Vulkan;
        //Let bgfx auto select adapter.
        config.vendorId = BGFX_PCI_ID_NONE;
        i32 width, height;
        SDL_GetWindowSize(Window::windowHandle, &width, &height);
        config.resolution.width = width;
        config.resolution.height = height;
        config.resolution.reset = BGFX_RESET_VSYNC;
        config.platformData = pdata;
        if (!bgfx::init(config)) {
            lerr << "[Renderer] Failed to initialize bgfx!" << endl;
            Fatal::exit(Fatal::BGFX_INITIALIZATION_FALILED);
        }
        lout << "Using rendering backend: " << bgfx::getRendererName(bgfx::getRendererType()) << endl;
        auto caps = bgfx::getCaps();

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
        using Components::BlockRenderComponent, Components::BlockCoordinatesComponent;
        using namespace Components;
        lout << "Renderer" << flush;
        lout << "Hello from renderer thread!" << endl;
        lout << "Initializing..." << endl;
        initBGFX();
        Gui::init();
        while (CherryGrove::isCGAlive) {
        //Process renderer-cycle input events.
            //InputHandler::processTrigger();
        //Prepare for rendering
            //Refresh windowHandle size
            if (sizeUpdateSignal) {
                SDL_GetWindowSize(Window::windowHandle, &cache.width, &cache.height);
                cache.aspectRatio = SDL_GetWindowDisplayScale(Window::windowHandle);
                bgfx::reset(cache.width, cache.height);
                sizeUpdateSignal = false;
            }
            //Temp debug code
            bgfx::setDebug(BGFX_DEBUG_STATS | BGFX_DEBUG_PROFILER | BGFX_DEBUG_TEXT);
        //Render GUI
            bgfx::setViewClear(guiViewId, BGFX_CLEAR_NONE, 0x00000000);
            bgfx::setViewRect(guiViewId, 0, 0, cache.width, cache.height);
            Gui::render();
        //Render game content
            bgfx::setViewClear(gameViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x5bd093ff);
            bgfx::setViewRect(gameViewId, 0, 0, cache.width, cache.height);
            if (Simulation::gameStarted) {
            //Prepare render environment
                float view[16]{}, proj[16]{};
                Rotation::getViewMtx(view, playerEntity);
                Camera::getProjMtx(proj, playerEntity, cache.aspectRatio);
                bgfx::setViewTransform(gameViewId, view, proj);
            //Render opaque parts/blocks?
                //Wait for the lock
                //unique_lock lock(Simulation::registryMutex);
                //Get all renderable blocks
                auto group = gameRegistry.group<const BlockCoordinatesComponent, const BlockRenderComponent>();
                group.each([](entt::entity entity, const BlockCoordinatesComponent& coords, const BlockRenderComponent& renderData) {
                    float worldSpaceTranslate[16]{};
                    bx::mtxTranslate(worldSpaceTranslate, (float)coords.x, (float)coords.y, (float)coords.z);
                    for (const auto& [cubeIndex, cube] : renderData.subcubes) {
                        float subcubeTransform[16]{};
                        //Subcube translate
                        bx::mtxTranslate(subcubeTransform, cube.origin.x, cube.origin.y, cube.origin.z);
                        //todo: Subcube rotation
                        //bx::mtxRotateXYZ(transform, );
                        for (i32 i = 0; i < 6; i++) if(cube.faces[i].shaderId) {
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
                //lock.unlock();
            //Render translucent blocks

            }
            //We must remind bgfx of view 0 actually exists, or we will be in trouble.
            //We don't need to check the game's status. In the early development, it's really easy to make a mistake and not submitting any draw calls to this view.
            bgfx::touch(gameViewId);
        //Update screen!
            bgfx::frame();
        }
        lout << "Terminating renderer thread!" << endl;
        Gui::shutdown();
        TexturePool::shutdown();
        ShaderPool::shutdown();
        bgfx::shutdown();
    }
}
