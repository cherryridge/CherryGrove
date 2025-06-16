#include <atomic>
#include <filesystem>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>

#include "../debug/Logger.hpp"
#include "../debug/Fatal.hpp"
#include "../CherryGrove.hpp"
#include "Sound.hpp"

namespace Sound {
    using std::atomic, std::memory_order_relaxed, std::memory_order_acquire, std::memory_order_release, std::filesystem::exists, std::thread, std::shared_mutex, std::unique_ptr, std::make_unique, std::scoped_lock, std::shared_lock, std::move, std::vector, std::filesystem::is_regular_file, SoLoud::SO_NO_ERROR, SoLoud::Soloud, Components::CoordinatesComp;
    static void audioLoop() noexcept;

    unique_ptr<Soloud> soLoudInstance;
    static thread audioThread;

    static SlotTable<SoundSource> soundRegistry;
    static SlotTable<PlayInfo> playRegistry;

    //`0` are invalid IDs.
    
    //id < `playStartHead` should be playing or ended. `playStartHead` is expected to be equal to `nextPlayId` after Audio Thread's work section.
    //It will only ever be changed by Audio Thread, so we don't need to atomic it.
    static PlayID playStartHead = 1;

    static CoordinatesComp receiverCoordsCache;
    static shared_mutex coordsMutex;

    void init() noexcept {
        soLoudInstance = make_unique<Soloud>();
        if (soLoudInstance->init() != SO_NO_ERROR) {
            lerr << "[Sound] SoLoud initialization failed!" << endl;
            Fatal::exit(Fatal::SOLOUD_INITIALIZATION_FALILED);
        }
        lout << soLoudInstance->getBackendString() << ": " << soLoudInstance->getBackendChannels() << " chan, buf: " << soLoudInstance->getBackendBufferSize() << endl;
        lout << "Creating audio thread..." << endl;
        audioThread = thread(&audioLoop);
        CherryGrove::subsystemSetupLatch.count_down();
    }

    void updateReceiverCoords(const CoordinatesComp& newCoords) noexcept {
        scoped_lock lock(coordsMutex);
        receiverCoordsCache = newCoords;
    }

    void updateSoundSpeed(float newSpeed) noexcept {
        //todo
    }

    void shutdown() noexcept {
        audioThread.join();
        soLoudInstance->deinit();
    }

//Sound Source
    SoundID addSound(const char* filePath, bool stream) noexcept {
        const auto id = nextSoundId.fetch_add(1, memory_order_relaxed);
        soundRegistry.emplace(id, filePath, stream);
        nextSoundId.store(id + 1, memory_order_release);
        return id;
    }

    bool removeSound(SoundID soundId) noexcept {
        const auto p = soundRegistry.find(soundId);
        if (p == soundRegistry.end()) return false;
        scoped_lock eventLock(eventMutex), playLock(playMutex);
        for (const auto& [eventId, event] : eventRegistry) if(event.soundId == soundId) {
            for (const auto& [playId, playInfo] : playRegistry) if (playInfo.eventId == eventId) {
                if (playInfo.progress < 1.0f) {
                    //todo: stop the play first, then delete the play data.
                }
            }
        }
        soundRegistry.erase(p);
        return true;
    }

//SoundEvent
    EventID addEvent(SoundID soundId, float volume, float pitch, float playSpeed, float maxDistance, u32 playCount, AttenuationAlgorithm atnl, bool useDoppler, bool is2D) noexcept {
        const auto id = nextEventId.fetch_add(1, memory_order_relaxed);
        const auto p = soundRegistry.find(soundId);
        if (p == soundRegistry.end()) {
            lerr << "[Sound] SoundID " << soundId << " not found!" << endl;
            return 0;
        }
        eventRegistry.emplace(id, soundId, volume, pitch, playSpeed, maxDistance, playCount, atnl, useDoppler, is2D);
        nextEventId.store(id + 1, memory_order_release);
        return id;
    }

    bool removeEvent(EventID eventId) {
        const auto p = eventRegistry.find(eventId);
        if (p == eventRegistry.end()) return false;
        scoped_lock playLock(playMutex);
        for (const auto& [playId, playInfo] : playRegistry) if (playInfo.eventId == eventId) {
            //todo: stop the play first, then delete the play data.
        }
        eventRegistry.erase(p);
        return true;
    }

//Play
    PlayID play(EventID eventId, CoordinatesComp coordinate, float initialProgress = 0.0, u32 loopCount) noexcept {
        const auto id = nextPlayId.fetch_add(1, memory_order_relaxed);
        const auto p = eventRegistry.find(eventId);
        if (p == eventRegistry.end()) {
            lerr << "[Sound] EventID " << eventId << " not found!" << endl;
            return 0;
        }
        const PlayInfo play{eventId, coordinate, initialProgress, loopCount};
        playRegistry.emplace(nextPlayId, play);
        nextPlayId.store(id + 1, memory_order_release);
        return id;
    }

    //Outside threads can never delete a play, because it may cause Audio Thread more time each audio frame to check if the play is still there.
    bool stop(PlayID playId) noexcept {
        const auto p = playRegistry.find(playId);
        if (p == playRegistry.end()) return false;

        return true;
    }

    bool pause(PlayID playId) noexcept {

    }

    bool resume(PlayID playId, float progress) noexcept {

    }

//Audio Thread
    //todo:combine it into audio loop
    //static void update(CoordinatesComp receiverCoord) noexcept {
    //    //todo: Put new arguments to every 3D sound.
    //    //todo: Manual background ticking & kill sound that exists in a different dimension
    //    soLoudInstance->update3dAudio();
    //}

    static void audioLoop() noexcept {
        lout << "Audio" << flush;
        lout << "Hello from audio thread!" << endl;
        while (CherryGrove::isCGAlive) {
            const auto _nextPlayId = nextPlayId.load(memory_order_acquire);
            for (; playStartHead < _nextPlayId; playStartHead++) {
                const auto& playInfo = playRegistry.find(playStartHead)->second;
                const auto& eventInfo = eventRegistry.find(playInfo.eventId)->second;
                auto& soundSource = soundRegistry.find(eventInfo.soundId)->second;
                if (eventInfo.is2D) {
                    const SoLoud::handle handle = soLoudInstance->playBackground(soundSource.wav);
                }
                else {

                }
            }
            //Audio thread will block on this line if the main thread is committing new play data.
            unique_lock lock(queueMutex);
            //Not necessary to introduce CVs.
            audioSignal.wait(lock, []() { return !playStartQueue.empty() || !CherryGrove::isCGAlive; });
            if (!CherryGrove::isCGAlive) break;
            const PlayID playId = playStartQueue.front();
            playStartQueue.pop();
            lock.unlock();
            const PlayInfo& play = playRegistry[playId];
            const SoundEvent& sEvent = eventRegistry[play.eventId];
            const auto& soundInst = soundRegistry[sEvent.soundId];
            visit([&play, &sEvent](auto& soundInst) {
                if (sEvent.is2D) {
                    const SoLoud::handle handle = soLoudInstance->play(*soundInst, sEvent.volume);
                }
                else {
                    //todo: Play 2D
                }
                //todo: Should implement background ticking manually. SoLoud only provide the ability to pause and resume the sound automatically, but not resuming at a deltatime jump in progress. We need to manually jump.
            }, soundInst);
            lout << "Started: Play " << playId << ", Event " << play.eventId << ", Sound " << sEvent.soundId << endl;
        }
        lout << "Terminating audio thread!" << endl;
    }

    void test() noexcept {
        auto soundtest = addSound("test/a.ogg");
        auto sound2 = addSound("test/b.ogg");
        auto soundevent1 = addEvent(soundtest, 1.0f, 1.0f, 1.0f, true);
        auto play1 = play(soundevent1);
        //Sleep(500);
        //auto play2 = play(soundevent1);
        auto soundevent2 = addEvent(soundtest, 0.3f, 1.0f, 1.0f, true);
        //Sleep(500);
        //auto play3 = play(soundevent2);
        auto soundevent223 = addEvent(sound2, 3.0f, 1.0f, 1.0f, true);
        //auto play4 = play(soundevent223);
    }
}