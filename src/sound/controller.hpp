#pragma once
#include <atomic>
#include <memory>
#include <thread>
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>
#include <soloud/soloud_audiosource.h>

#include "../debug/Fatal.hpp"
#include "../debug/Logger.hpp"
#include "hold.hpp"
#include "soLoudInstance.hpp"

namespace Sound {
    using std::atomic, std::thread, std::memory_order_acquire, std::memory_order_release, std::this_thread::yield, std::make_unique, SoLoud::Soloud;

    namespace detail {
        inline thread audioThread;
        inline atomic<bool> initialized{false};

        inline void shutdown() noexcept {
            lout << "Terminating audio thread!" << endl;
            initialized.store(false, memory_order_release);
            soLoudInstance->deinit();
        }

        inline void init() noexcept {
            lout << "Audio" << flush;
            lout << "Hello from audio thread!" << endl;
            soLoudInstance = make_unique<Soloud>();
            const auto code = soLoudInstance->init(Soloud::CLIP_ROUNDOFF, Soloud::AUTO);
            if (code != SoLoud::SO_NO_ERROR) {
                lerr << "[Sound] SoLoud initialization failed after trying available backends: " << soLoudInstance->getErrorString(code) << endl;
                Fatal::exit(Fatal::SOLOUD_INITIALIZATION_FAILED);
            }
            lout << "[Sound] Backend: " << soLoudInstance->getBackendString() << ", channels: " << soLoudInstance->getBackendChannels() << ", bufsize: " << soLoudInstance->getBackendBufferSize() << endl;
            initialized.store(true, memory_order_release);
            hold();
            shutdown();
        }
    }

    //threaded: Main Thread
    inline void init() noexcept {
        detail::audioThread = thread(detail::init);
        while (!detail::initialized.load(memory_order_acquire)) yield();
    }

    //threaded: Main Thread
    inline void shutdown() noexcept {
        if (detail::audioThread.joinable()) detail::audioThread.join();
    }
}