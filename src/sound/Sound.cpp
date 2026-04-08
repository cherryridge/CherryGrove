#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include <glm/glm.hpp>
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>
#include <soloud/soloud_audiosource.h>

#include "../debug/Logger.hpp"
#include "../debug/Fatal.hpp"
#include "../globalState.hpp"
#include "../util/concurrentQueue.hpp"
#include "../util/os/platform.hpp"
#include "commands.hpp"
#include "SoundSource.hpp"
#include "PlayInfo.hpp"
#include "types.hpp"

#include "Sound.hpp"

//threaded: Audio Thread
namespace Sound {
    using std::atomic, std::this_thread::yield, std::atomic_ref, std::memory_order_acquire, std::memory_order_release, std::thread, std::unique_ptr, std::make_unique, std::vector, SoLoud::SO_NO_ERROR, SoLoud::Soloud, Components::CoordinatesComp, Components::RotationComp, Components::VelocityComp, glm::radians, std::chrono::steady_clock, std::chrono::duration_cast, std::chrono::microseconds, Util::MPSCQueue;
    static void audioLoop() noexcept;

    atomic<bool> initialized{false};
    u64 perf_LoopTimeUs{0};
    unique_ptr<Soloud> soLoudInstance;
    SlotTableForAudio<SoundHandle, SoundSource> soundRegistry;
    SlotTableForAudio<PlayHandle, PlayInfo> playRegistry;
    static MPSCQueue<Command> commandQueue;
    static vector<PlayHandle> loopPlays;
    static thread audioThread;

    //threaded: Main Thread
    void init() noexcept {
        audioThread = thread(&audioLoop);
        while (!initialized.load(memory_order_acquire)) yield();
    }
    void shutdown() noexcept { audioThread.join(); }

//Global State

    void updateListenerPosition(const CoordinatesComp& position) noexcept { commandQueue.enqueue(GlobalPosition(position)); }
    void updateListenerRotation(const RotationComp& rotation) noexcept { commandQueue.enqueue(GlobalRotation(rotation)); }
    void updateListenerVelocity(const VelocityComp& velocity) noexcept { commandQueue.enqueue(GlobalVelocity(velocity)); }
    void updateSoundSpeed(float speed) noexcept { commandQueue.enqueue(speed); }

//SoundSource

    SoundHandle addSound(const char* filePath, bool isStream, bool is2D, float volume, float maxDistance, float minDistance, float dopplerFactor, Attenuation attn, InaudibleBehavior iabh, float rolloff, bool soundSpeedDelay, bool fastPlay) noexcept {
        atomic<bool> finished{false};
        SoundHandle result{GenerationalHandle(0)};
        commandQueue.enqueue(AddSource(filePath, isStream, is2D, attn, iabh, volume, maxDistance, minDistance, dopplerFactor, rolloff, result, finished, soundSpeedDelay, fastPlay));
        //todo: Implement an event/promise mechanism for every thread to send and receive messages instead of dead waiting for Audio Thread.
        while (!finished.load(memory_order_acquire)) yield();
        return result;
    }

    bool deleteSound(SoundHandle handle) noexcept {
        atomic<bool> finished{false};
        bool success = false;
        commandQueue.enqueue(DeleteSource(handle, success, finished));
        //todo: Implement an event/promise mechanism for every thread to send and receive messages instead of dead waiting for Audio Thread.
        while (!finished.load(memory_order_acquire)) yield();
        return success;
    }

//PlayInfo

    PlayHandle play(SoundHandle soundHandle, const CoordinatesComp& position, const VelocityComp& velocity, u32 playCount, float iniProgress, float pitch, float playSpeed) noexcept {
        atomic<bool> finished{false};
        PlayHandle result{GenerationalHandle(0)};
        commandQueue.enqueue(Play(soundHandle, position, velocity, iniProgress, pitch, playSpeed, result, finished, playCount));
        //todo: Implement an event/promise mechanism for every thread to send and receive messages instead of dead waiting for Audio Thread.
        while (!finished.load(memory_order_acquire)) yield();
        return result;
    }

    bool pause(PlayHandle handle) noexcept {
        atomic<bool> finished{false};
        bool success = false;
        commandQueue.enqueue(Pause(handle, success, finished));
        //todo: Implement an event/promise mechanism for every thread to send and receive messages instead of dead waiting for Audio Thread.
        while (!finished.load(memory_order_acquire)) yield();
        return success;
    }

    bool resume(PlayHandle handle, float progress) noexcept {
        atomic<bool> finished{false};
        bool success = false;
        commandQueue.enqueue(Resume(handle, success, finished, progress));
        //todo: Implement an event/promise mechanism for every thread to send and receive messages instead of dead waiting for Audio Thread.
        while (!finished.load(memory_order_acquire)) yield();
        return success;
    }

    bool stop(PlayHandle handle) noexcept {
        atomic<bool> finished{false};
        bool success = false;
        commandQueue.enqueue(Stop(handle, success, finished));
        //todo: Implement an event/promise mechanism for every thread to send and receive messages instead of dead waiting for Audio Thread.
        while (!finished.load(memory_order_acquire)) yield();
        return success;
    }

    bool updateSourcePosition(PlayHandle handle, const CoordinatesComp& position) noexcept {
        atomic<bool> finished{false};
        bool success = false;
        commandQueue.enqueue(SourcePosition(handle, position, success, finished));
        //todo: Implement an event/promise mechanism for every thread to send and receive messages instead of dead waiting for Audio Thread.
        while (!finished.load(memory_order_acquire)) yield();
        return success;
    }

    bool updateSourceVelocity(PlayHandle handle, const VelocityComp& velocity) noexcept {
        atomic<bool> finished{false};
        bool success = false;
        commandQueue.enqueue(SourceVelocity(handle, velocity, success, finished));
        //todo: Implement an event/promise mechanism for every thread to send and receive messages instead of dead waiting for Audio Thread.
        while (!finished.load(memory_order_acquire)) yield();
        return success;
    }

//Audio Thread

    static void initSoLoud() noexcept {
        soLoudInstance = make_unique<Soloud>();
        #if CG_PLATFORM_WINDOWS
            const auto preferredBackend = Soloud::XAUDIO2;
        #elif CG_PLATFORM_LINUX
            const auto preferredBackend = Soloud::JACK;
        #elif CG_PLATFORM_MACOS
            const auto preferredBackend = Soloud::COREAUDIO;
        #elif CG_PLATFORM_ANDROID
            const auto preferredBackend = Soloud::OPENSLES;
        #elif CG_PLATFORM_IOS
            const auto preferredBackend = Soloud::COREAUDIO;
        #endif
        const auto code = soLoudInstance->init(SoLoud::Soloud::CLIP_ROUNDOFF, preferredBackend);
        if (code != SO_NO_ERROR) {
            lerr << "[Sound] SoLoud initialization failed: " << soLoudInstance->getErrorString(code) << endl;
            Fatal::exit(Fatal::SOLOUD_INITIALIZATION_FAILED);
        }
        lout << "[Sound] Backend: " << soLoudInstance->getBackendString() << ", channels: " << soLoudInstance->getBackendChannels() << ", bufsize: " << soLoudInstance->getBackendBufferSize() << endl;
    }

    static void audioLoop() noexcept {
        lout << "Audio" << flush;
        lout << "Hello from audio thread!" << endl;
        initSoLoud();
        initialized.store(true, memory_order_release);
        u32 gcCursor = 0;
        while (GlobalState::isCGAlive) {
            auto startTime = steady_clock::now();
            bool shouldUpdate3D = false;
        //Process commands.
            Command command;
            u8 dequeueCounter = 0;
            if (++dequeueCounter <= MAX_DEQUEUE_PER_LOOP && commandQueue.dequeue(command)) switch (command.type) {
                case Command::Type::AddSource: {
                    const auto& [handle, ptr] = soundRegistry.emplace(command.addSource.filePath, command.addSource.isStream, command.addSource.is2D, command.addSource.volume, command.addSource.maxDistance, command.addSource.minDistance, command.addSource.dopplerFactor, command.addSource.attn, command.addSource.iabh, command.addSource.rolloff, command.addSource.soundSpeedDelay, command.addSource.fastPlay);
                    if (ptr->active) command.addSource.result = handle;
                    else {
                        command.addSource.result = SoundHandle{GenerationalHandle(0)};
                        static_cast<void>(soundRegistry.destroy(handle));
                    }
                    command.addSource.finished.store(true, memory_order_release);
                    break;
                }
                //Why we don't stop and destroy every play that depends on this source:
                //1. They all hold reference to data, so no crashes.
                //2. It's the upper-level systems' responsibility to clean up everything.
                //3. It's tricky to implement.
                //4. Sounds are often short and one-shot except for BGM, so 99% of the time the mechanism will be useless.
                case Command::Type::DeleteSource: {
                    command.deleteSource.success = soundRegistry.destroy(command.deleteSource.h);
                    command.deleteSource.finished.store(true, memory_order_release);
                    break;
                }
                case Command::Type::Play: {
                    const auto& [handle, ptr] = playRegistry.emplace(command.play.soundHandle, command.play.position, command.play.velocity, command.play.playCount, command.play.pitch, command.play.playSpeed);
                    if (ptr->active && ptr->play()) {
                        command.play.result = handle;
                        if (command.play.playCount > 1) loopPlays.push_back(handle);
                    }
                    else {
                        command.play.result = PlayHandle{GenerationalHandle(0)};
                        static_cast<void>(playRegistry.destroy(handle));
                    }
                    command.play.finished.store(true, memory_order_release);
                    break;
                }
                case Command::Type::Pause: {
                    const auto ptr = playRegistry.getPtr(command.pause.h);
                    command.pause.success = ptr && ptr->pause();
                    command.pause.finished.store(true, memory_order_release);
                    break;
                }
                case Command::Type::Resume: {
                    const auto ptr = playRegistry.getPtr(command.resume.h);
                    command.resume.success = ptr && ptr->resume(command.resume.progress);
                    command.resume.finished.store(true, memory_order_release);
                    break;
                }
                case Command::Type::Stop: {
                    const auto ptr = playRegistry.getPtr(command.stop.h);
                    command.stop.success = ptr && ptr->stop() && playRegistry.destroy(command.stop.h);
                    command.stop.finished.store(true, memory_order_release);
                    break;
                }
                case Command::Type::GlobalPosition: {
                    soLoudInstance->set3dListenerPosition(static_cast<float>(command.globalPosition.position.x), static_cast<float>(command.globalPosition.position.y), static_cast<float>(command.globalPosition.position.z));
                    shouldUpdate3D = true;
                    break;
                }
                case Command::Type::GlobalRotation: {
                    soLoudInstance->set3dListenerAt(
                        static_cast<float>(sin(radians(command.globalRotation.rotation.yaw)) * cos(radians(command.globalRotation.rotation.pitch))),
                        static_cast<float>(sin(radians(command.globalRotation.rotation.pitch))),
                        static_cast<float>(cos(radians(command.globalRotation.rotation.yaw)) * cos(radians(command.globalRotation.rotation.pitch)))
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
                    if (ptr && ptr->active && ptr->playActive()) {
                        soLoudInstance->set3dSourcePosition(ptr->instanceHandle, static_cast<float>(command.sourcePosition.position.x), static_cast<float>(command.sourcePosition.position.y), static_cast<float>(command.sourcePosition.position.z));
                        command.sourcePosition.success = true;
                        shouldUpdate3D = true;
                    }
                    else command.sourcePosition.success = false;
                    command.sourcePosition.finished.store(true, memory_order_release);
                    break;
                }
                case Command::Type::SourceVelocity: {
                    const auto ptr = playRegistry.getPtr(command.sourceVelocity.h);
                    if (ptr && ptr->active && ptr->playActive()) {
                        soLoudInstance->set3dSourceVelocity(ptr->instanceHandle, command.sourceVelocity.velocity.dx, command.sourceVelocity.velocity.dy, command.sourceVelocity.velocity.dz);
                        command.sourceVelocity.success = true;
                        shouldUpdate3D = true;
                    }
                    else command.sourceVelocity.success = false;
                    command.sourceVelocity.finished.store(true, memory_order_release);
                    break;
                }
                default: {
                    lerr << "[Sound] WTF is command type " << static_cast<u8>(command.type) << "?" << endl;
                    break;
                }
            }
        //Check for looping plays. All looping plays must be check in one go because less frequency means more likely to overshoot.
            //Note: We have to use iterator because we might erase elements.
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
            perf_LoopTimeUs = duration_cast<microseconds>(endTime - startTime).count();
        }
        lout << "Terminating audio thread!" << endl;
        initialized.store(false, memory_order_release);
        soLoudInstance->deinit();
    }
}