#pragma once
#include <chrono>

#include "../globalState.hpp"
#include "../intrinsics/systems/Rotation.hpp"
#include "command.hpp"
#include "commandQueue.hpp"
#include "configs.hpp"
#include "debug.hpp"
#include "playRegistry.hpp"
#include "SlotTableForAudio.hpp"
#include "soundRegistry.hpp"

namespace Sound::detail {
    typedef uint32_t u32;
    using std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds;

    inline vector<PlayHandle> loopPlays;

    inline void hold() noexcept {
        u32 gcCursor = 0;
        while (GlobalState::isCGAlive()) {
            auto startTime = steady_clock::now();

            bool shouldUpdate3D = false;
        //Process commands.
            Command command;
            u8 dequeueCounter = 0;
            if (++dequeueCounter <= MAX_DEQUEUE_PER_LOOP && commandQueue.dequeue(command)) switch (command.type) {
                case Command::Type::AddSource: {
                    const auto& [handle, ptr] = soundRegistry.emplace(command.addSource.filePath, command.addSource.isStream, command.addSource.is2D, command.addSource.volume, command.addSource.maxDistance, command.addSource.minDistance, command.addSource.dopplerFactor, command.addSource.attn, command.addSource.iabh, command.addSource.rolloff, command.addSource.soundSpeedDelay, command.addSource.fastPlay);
                    SoundHandle result;
                    if (ptr->active) result = handle;
                    else {
                        result = SoundHandle{GenerationalHandle(0)};
                        static_cast<void>(soundRegistry.destroy(handle));
                    }
                    if (command.addSource.promise) command.addSource.promise->fulfill(result);
                    break;
                }
                //Why we don't stop and destroy every play that depends on this source:
                //1. They all hold reference to data, so no crashes.
                //2. It's the upper-level systems' responsibility to clean up everything.
                //3. It's tricky to implement.
                //4. Sounds are often short and one-shot except for BGM, so 99% of the time the mechanism will be useless.
                case Command::Type::DeleteSource: {
                    const bool success = soundRegistry.destroy(command.deleteSource.h);
                    if (command.deleteSource.promise) command.deleteSource.promise->fulfill(success);
                    break;
                }
                case Command::Type::Play: {
                    const auto& [handle, ptr] = playRegistry.emplace(command.play.soundHandle, command.play.position, command.play.velocity, command.play.playCount, command.play.pitch, command.play.playSpeed);
                    PlayHandle result;
                    if (ptr->active && ptr->play()) {
                        result = handle;
                        if (command.play.playCount > 1) loopPlays.push_back(handle);
                    }
                    else {
                        result = PlayHandle{GenerationalHandle(0)};
                        static_cast<void>(playRegistry.destroy(handle));
                    }
                    if (command.play.promise) command.play.promise->fulfill(result);
                    break;
                }
                case Command::Type::Pause: {
                    const auto ptr = playRegistry.getPtr(command.pause.h);
                    const bool success = ptr && ptr->pause();
                    if (command.pause.promise) command.pause.promise->fulfill(success);
                    break;
                }
                case Command::Type::Resume: {
                    const auto ptr = playRegistry.getPtr(command.resume.h);
                    const bool success = ptr && ptr->resume(command.resume.progress);
                    if (command.resume.promise) command.resume.promise->fulfill(success);
                    break;
                }
                case Command::Type::Stop: {
                    const auto ptr = playRegistry.getPtr(command.stop.h);
                    const bool success = ptr && ptr->stop() && playRegistry.destroy(command.stop.h);
                    if (command.stop.promise) command.stop.promise->fulfill(success);
                    break;
                }
                case Command::Type::GlobalPosition: {
                    soLoudInstance->set3dListenerPosition(static_cast<float>(command.globalPosition.position.x), static_cast<float>(command.globalPosition.position.y), static_cast<float>(command.globalPosition.position.z));
                    shouldUpdate3D = true;
                    break;
                }
                case Command::Type::GlobalRotation: {
                    const auto direction = Systems::getUnitVecFromRotation(command.globalRotation.rotation);
                    soLoudInstance->set3dListenerAt(
                        direction.x,
                        direction.y,
                        direction.z
                    );
                    shouldUpdate3D = true;
                    break;
                }
                case Command::Type::GlobalVelocity: {
                    soLoudInstance->set3dListenerVelocity(command.globalVelocity.velocity.dx, command.globalVelocity.velocity.dy, command.globalVelocity.velocity.dz);
                    shouldUpdate3D = true;
                    break;
                }
                case Command::Type::UpdateSoundSpeed: {
                    soLoudInstance->set3dSoundSpeed(command.updateSoundSpeed);
                    shouldUpdate3D = true;
                    break;
                }
                case Command::Type::SourcePosition: {
                    const auto ptr = playRegistry.getPtr(command.sourcePosition.h);
                    bool success = false;
                    if (ptr && ptr->active && ptr->playActive()) {
                        soLoudInstance->set3dSourcePosition(ptr->instanceHandle, static_cast<float>(command.sourcePosition.position.x), static_cast<float>(command.sourcePosition.position.y), static_cast<float>(command.sourcePosition.position.z));
                        success = true;
                        shouldUpdate3D = true;
                    }
                    if (command.sourcePosition.promise) command.sourcePosition.promise->fulfill(success);
                    break;
                }
                case Command::Type::SourceVelocity: {
                    const auto ptr = playRegistry.getPtr(command.sourceVelocity.h);
                    bool success = false;
                    if (ptr && ptr->active && ptr->playActive()) {
                        soLoudInstance->set3dSourceVelocity(ptr->instanceHandle, command.sourceVelocity.velocity.dx, command.sourceVelocity.velocity.dy, command.sourceVelocity.velocity.dz);
                        success = true;
                        shouldUpdate3D = true;
                    }
                    if (command.sourceVelocity.promise) command.sourceVelocity.promise->fulfill(success);
                    break;
                }
                default: {
                    lerr << "[Sound] WTF is command type " << static_cast<u8>(command.type) << "?" << endl;
                    break;
                }
            }
        //Check for looping plays. All looping plays must be check in one go because less frequency means more likely to overshoot.
            //note: We have to use iterator because we might erase elements.
            for (auto it = loopPlays.begin(); it != loopPlays.end();) {
                const auto ptr = playRegistry.getPtr(*it);
                if (ptr) {
                    if (!ptr->playActive()) it = loopPlays.erase(it);
                    else {
                        auto count = soLoudInstance->getLoopCount(ptr->instanceHandle);
                        if (count == ptr->playCount) {
                            soLoudInstance->setLooping(ptr->instanceHandle, false);
                            it = loopPlays.erase(it);
                        }
                        else if (count > ptr->playCount) {
                            static_cast<void>(ptr->stop());
                            static_cast<void>(playRegistry.destroy(*it));
                            it = loopPlays.erase(it);
                        }
                        else ++it;
                    }
                }
                else it = loopPlays.erase(it);
            }
        //Garbage collect stopped plays.
            const auto size = playRegistry.storage.size();
            if (size != 0) for (u32 i = 0; i < MAX_GC_PER_LOOP; i++) {
                gcCursor++;
                //Loop back.
                if (gcCursor == size) gcCursor = 0;
                const auto& entry = playRegistry.storage[gcCursor];
                //`data` is null iff the slot is in freeList (both set by `destroy`, both cleared by `emplace`, single-threaded on Audio Thread).
                //In fact there is only one scenario we're catching: Play ends naturally. By filtering data we indirectly filter out every slot that was already destroyed and emplaced to `freeList`.
                if (entry.data && !entry.data->playActive()) static_cast<void>(playRegistry.destroy(PlayHandle{GenerationalHandle(
                    atomic_ref(entry.generation).load(memory_order_acquire),
                    gcCursor
                )}));
            }
        //Update 3D audio if needed.
            if (shouldUpdate3D) soLoudInstance->update3dAudio();

            const auto endTime = steady_clock::now();
            Sound::perf_audioUSPT.store(duration_cast<microseconds>(endTime - startTime).count(), memory_order_release);
        }
    }
}