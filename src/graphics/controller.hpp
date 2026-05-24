#pragma once
#include <atomic>
#include <thread>

#include "../debug/loggers.hpp"
#include "gui/Gui.hpp"
#include "renderer/Renderer.hpp"
#include "shader/ShaderPool.hpp"
#include "texture/TexturePool.hpp"
#include "hold.hpp"

namespace Graphics {
    typedef uint64_t u64;
    using std::atomic, std::memory_order_release, std::memory_order_acquire, std::thread, std::this_thread::yield;

    namespace detail {
        inline atomic<bool> initialized{false}, shutdownComplete{false};
        inline thread graphicsThread;

        inline void shutdown() noexcept {
            lout << "Terminating Graphics thread!" << nlaf;
            initialized.store(false, memory_order_release);
            Gui::shutdown();
            TexturePool::shutdown();
            ShaderPool::shutdown();
            Renderer::shutdown();
            shutdownComplete.store(true, memory_order_release);
        }

        inline void init() noexcept {
            Debug::setThreadName("Graphics");
            lout << "Hello from the brand-new graphics thread!" << nlaf;
            Renderer::init();
            ShaderPool::init();
            TexturePool::init();
            Gui::init();
            initialized.store(true, memory_order_release);
            hold();
            shutdown();
        }
    }

    //threaded: Main Thread
    inline void init() noexcept {
        //Call this on the SDL/main thread before `bgfx::init` so bgfx uses this thread as its render thread.
        bgfx::renderFrame();
        detail::graphicsThread = thread(detail::init);
    }

    [[nodiscard]] inline bool isInitialized() noexcept {
        return detail::initialized.load(memory_order_acquire);
    }

    //threaded: Main Thread
    inline void shutdown() noexcept {
        if (!detail::graphicsThread.joinable()) return;
        while (!detail::shutdownComplete.load(memory_order_acquire)) bgfx::renderFrame();
        detail::graphicsThread.join();
    }
}