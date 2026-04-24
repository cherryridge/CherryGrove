#pragma once
#include <atomic>
#include <thread>

#include "../debug/Logger.hpp"
#include "gui/Gui.hpp"
#include "renderer/Renderer.hpp"
#include "shader/ShaderPool.hpp"
#include "texture/TexturePool.hpp"
#include "hold.hpp"

namespace Graphics {
    typedef uint64_t u64;
    using std::atomic, std::memory_order_release, std::memory_order_acquire, std::thread;

    namespace detail {
        inline atomic<bool> initialized{false}, shutdownComplete{false};
        inline thread graphicsThread;

        inline void initGraphicsThread() noexcept {
            lout << "Graphics" << flush;
            lout << "Hello from the brand-new graphics thread!" << endl;
            Renderer::init();
            ShaderPool::init();
            TexturePool::init();
            Gui::init();
            initialized.store(true, memory_order_release);
            hold();
            lout << "Terminating Graphics thread!" << endl;
            initialized.store(false, memory_order_release);
            Gui::shutdown();
            TexturePool::shutdown();
            ShaderPool::shutdown();
            Renderer::shutdown();
            shutdownComplete.store(true, memory_order_release);
        }
    }

    inline void init() noexcept {
        detail::graphicsThread = thread(detail::initGraphicsThread);
        while (!detail::initialized.load(memory_order_acquire)) bgfx::renderFrame();
    }

    inline void shutdown() noexcept {
        if (!detail::graphicsThread.joinable()) return;
        while (!detail::shutdownComplete.load(memory_order_acquire)) bgfx::renderFrame();
        detail::graphicsThread.join();
    }
}