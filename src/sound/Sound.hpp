#pragma once
#include <atomic>

#include "../components/Components.hpp"
#include "enums.hpp"
#include "types.hpp"

namespace Sound {
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::atomic, Components::CoordinatesComp, Components::RotationComp, Components::VelocityComp;

    extern u64 perf_audioUSPT;
    extern atomic<bool> initialized;

    inline constexpr u32 MAX_DEQUEUE_PER_LOOP = 16, MAX_GC_PER_LOOP = 64;
    inline CoordinatesComp DUMMY_COORD_2D = {0.0f, 0.0f, 0.0f, 0};

    void init() noexcept;
    //Must be called after `CherryGrove::isCGAlive == false`.
    void shutdown() noexcept;

    void updateListenerPosition(const CoordinatesComp& position) noexcept;
    void updateListenerRotation(const RotationComp& rotation) noexcept;
    void updateListenerVelocity(const VelocityComp& velocity) noexcept;
    void updateSoundSpeed(float speed) noexcept;

    [[nodiscard]] SoundHandle addSound(const char* filePath, bool isStream, bool is2D, float volume, float maxDistance, float minDistance, float dopplerFactor = 0.0f, Attenuation attn = Attenuation::Inverse, InaudibleBehavior iabh = InaudibleBehavior::PauseTick, float rolloff = 1.0f, bool soundSpeedDelay = false, bool fastPlay = false) noexcept;
    [[nodiscard]] bool deleteSound(SoundHandle handle) noexcept;

    [[nodiscard]] PlayHandle play(SoundHandle soundHandle, const CoordinatesComp& position, const VelocityComp& velocity = {0.0, 0.0, 0.0}, u32 playCount = 1, float iniProgress = 0.0f, float pitch = 1.0f, float playSpeed = 1.0f) noexcept;
    [[nodiscard]] bool pause(PlayHandle handle) noexcept;
    [[nodiscard]] bool resume(PlayHandle handle, float progress = FLOAT_INFINITY) noexcept;
    [[nodiscard]] bool stop(PlayHandle handle) noexcept;
    [[nodiscard]] bool updateSourcePosition(PlayHandle handle, const CoordinatesComp& position) noexcept;
    [[nodiscard]] bool updateSourceVelocity(PlayHandle handle, const VelocityComp& velocity) noexcept;
};