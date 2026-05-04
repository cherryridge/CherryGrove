#pragma once
#include <atomic>

#include "../intrinsics/components/Coordinates.hpp"
#include "../intrinsics/components/Rotation.hpp"
#include "../intrinsics/components/Velocity.hpp"
#include "../util/Promise.hpp"
#include "enums.hpp"
#include "types.hpp"

namespace Sound {
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::atomic, Util::Promise;

    extern u64 perf_audioUSPT;
    extern atomic<bool> initialized;

    inline constexpr u32 MAX_DEQUEUE_PER_LOOP = 16, MAX_GC_PER_LOOP = 64;
    inline constexpr Components::EntityCoordinates DUMMY_COORD_2D = {0.0, 0.0, 0.0, 0ull};

    void init() noexcept;
    void shutdown() noexcept;

//All public API below is asynchronous. Calls return as soon as the command is enqueued — the audio thread fulfills any caller-supplied promise once it processes the command.
//Promise lifetime contract: the Promise must be stored at a stable address (no `std::vector<Promise<T>>` — use `std::list` or a non-relocating pool) and must outlive command processing (i.e. until `tryTake()` succeeds or `wait()` returns). Pass `nullptr` for fire-and-forget.

    //Fire-and-forget. Arguments are copied by value into the command, so caller storage may go out of scope immediately.
    void updateListenerPosition(const Components::EntityCoordinates& position) noexcept;
    void updateListenerRotation(const Components::Rotation& rotation) noexcept;
    void updateListenerVelocity(const Components::Velocity& velocity) noexcept;
    void updateSoundSpeed(float speed) noexcept;

    void addSound(Promise<SoundHandle>* promise, const char* filePath, bool isStream, bool is2D, float volume, float maxDistance, float minDistance, float dopplerFactor = 0.0f, Attenuation attn = Attenuation::Inverse, InaudibleBehavior iabh = InaudibleBehavior::PauseTick, float rolloff = 1.0f, bool soundSpeedDelay = false, bool fastPlay = false) noexcept;
    void deleteSound(Promise<bool>* promise, SoundHandle handle) noexcept;

    void play(Promise<PlayHandle>* promise, SoundHandle soundHandle, const Components::EntityCoordinates& position, const Components::Velocity& velocity = {0.0, 0.0, 0.0}, u32 playCount = 1, float iniProgress = 0.0f, float pitch = 1.0f, float playSpeed = 1.0f) noexcept;
    void pause(Promise<bool>* promise, PlayHandle handle) noexcept;
    void resume(Promise<bool>* promise, PlayHandle handle, float progress = FLOAT_INFINITY) noexcept;
    void stop(Promise<bool>* promise, PlayHandle handle) noexcept;

    void updateSourcePosition(Promise<bool>* promise, PlayHandle handle, const Components::EntityCoordinates& position) noexcept;
    void updateSourceVelocity(Promise<bool>* promise, PlayHandle handle, const Components::Velocity& velocity) noexcept;
};