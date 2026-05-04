#pragma once
#include <atomic>

#include "../intrinsics/components/Coordinates.hpp"
#include "../intrinsics/components/Rotation.hpp"
#include "../intrinsics/components/Velocity.hpp"
#include "../util/Promise.hpp"
#include "command.hpp"
#include "commandQueue.hpp"
#include "types.hpp"

namespace Sound {
    typedef uint32_t u32;
    using std::atomic, Components::EntityCoordinates, Components::Rotation, Components::Velocity, Util::Promise;

//Fire-and-forget APIs. Arguments are copied by value into the command, so caller storage may go out of scope immediately.

    inline void updateListenerPosition(const EntityCoordinates& position) noexcept { detail::commandQueue.enqueue(detail::GlobalPosition{position}); }

    inline void updateListenerRotation(const Rotation& rotation) noexcept { detail::commandQueue.enqueue(detail::GlobalRotation{rotation}); }

    inline void updateListenerVelocity(const Velocity& velocity) noexcept { detail::commandQueue.enqueue(detail::GlobalVelocity{velocity}); }

    inline void updateSoundSpeed(float speed) noexcept { detail::commandQueue.enqueue(speed); }

//All public API below is asynchronous. Calls return as soon as the command is enqueued — the audio thread fulfills any caller-supplied promise once it processes the command.
//Promise lifetime contract: the Promise must be stored at a stable address (no `std::vector<Promise<T>>` — use `std::list` or a non-relocating pool) and must outlive command processing (i.e. until `tryTake()` succeeds or `wait()` returns). Pass `nullptr` for fire-and-forget.

    inline void addSound(
        Promise<SoundHandle>* promise,
        const char* filePath,
        bool isStream,
        bool is2D,
        float volume,
        float maxDistance,
        float minDistance,
        float dopplerFactor = 0.0f,
        Attenuation attn = Attenuation::Inverse,
        InaudibleBehavior iabh = InaudibleBehavior::PauseTick,
        float rolloff = 1.0f,
        bool soundSpeedDelay = false,
        bool fastPlay = false
    ) noexcept { detail::commandQueue.enqueue(detail::AddSource{
        filePath,
        isStream,
        is2D,
        attn,
        iabh,
        volume,
        maxDistance,
        minDistance,
        dopplerFactor,
        rolloff,
        promise,
        soundSpeedDelay,
        fastPlay
    }); }

    inline void deleteSound(Promise<bool>* promise, SoundHandle handle) noexcept { detail::commandQueue.enqueue(detail::DeleteSource{handle, promise}); }


    inline void play(
        Promise<PlayHandle>* promise,
        SoundHandle soundHandle,
        const EntityCoordinates& position,
        const Velocity& velocity = {0.0, 0.0, 0.0},
        u32 playCount = 1,
        float iniProgress = 0.0f,
        float pitch = 1.0f,
        float playSpeed = 1.0f
    ) noexcept { detail::commandQueue.enqueue(detail::Play{
        soundHandle,
        position.getGLMVec3(),
        velocity.getGLMVec3(),
        iniProgress,
        pitch,
        playSpeed,
        playCount,
        promise,
    }); }

    inline void pause(Promise<bool>* promise, PlayHandle handle) noexcept { detail::commandQueue.enqueue(detail::Pause{handle, promise}); }

    inline void resume(Promise<bool>* promise, PlayHandle handle, float progress = FLOAT_INFINITY) noexcept { detail::commandQueue.enqueue(detail::Resume{handle, promise, progress}); }

    inline void stop(Promise<bool>* promise, PlayHandle handle) noexcept { detail::commandQueue.enqueue(detail::Stop{handle, promise}); }


    inline void updateSourcePosition(Promise<bool>* promise, PlayHandle handle, const EntityCoordinates& position) noexcept { detail::commandQueue.enqueue(detail::SourcePosition{handle, position, promise}); }

    inline void updateSourceVelocity(Promise<bool>* promise, PlayHandle handle, const Velocity& velocity) noexcept { detail::commandQueue.enqueue(detail::SourceVelocity{handle, velocity, promise}); }
};