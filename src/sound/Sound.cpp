#include <unordered_map>
#include <memory>
#include <queue>
#include <mutex>
#include <filesystem>
#include <thread>
#include <variant>
#include <condition_variable>
#include <glm/glm.hpp>
#include <soloud/soloud.h>
#include <soloud/soloud_thread.h>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>

#include "../debug/Logger.hpp"
#include "../debug/Fatal.hpp"
#include "../CherryGrove.hpp"
#include "Sound.hpp"

namespace Sound {
    using std::unordered_map, SoLoud::Soloud, std::make_unique, std::unique_ptr, SoLoud::Wav, SoLoud::WavStream, std::filesystem::exists, std::thread, std::queue, std::mutex, std::variant, std::unique_lock, std::move, SoLoud::SO_NO_ERROR, std::condition_variable, std::filesystem::is_regular_file, Components::CoordinatesComponent;

    static void audioLoop();

    unique_ptr<Soloud> soLoudInstance;

    thread audioThread;
    queue<PlayID> playStartQueue;
    mutex queueMutex;
    condition_variable audioSignal;

    unordered_map<SoundID, variant<unique_ptr<Wav>, unique_ptr<WavStream>>> soundRegistry;
    unordered_map<EventID, SoundEvent> eventRegistry;
    unordered_map<PlayID, PlayInfo> playInfos;
    //We are going to use `0` as invalid ID.
    SoundID nextSoundId = 1;
    EventID nextEventId = 1;
    PlayID nextPlayId = 1;

    void init() {
        soLoudInstance = make_unique<Soloud>();
        if (soLoudInstance->init(Soloud::LEFT_HANDED_3D) != SO_NO_ERROR) {
            lerr << "[Sound] Initialization failed!" << endl;
            Fatal::exit(Fatal::SOLOUD_INITIALIZATION_FALILED);
        }
        lout << "SoLoud is running on " << soLoudInstance->getBackendString() << " with " << soLoudInstance->getBackendChannels() << " channels." << endl;
        lout << "Creating audio thread..." << endl;
        audioThread = thread(&audioLoop);
        CherryGrove::subsystemLatch.count_down();
    }

    void test() {
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

    void shutdown() {
        audioSignal.notify_one();
        audioThread.join();
        soundRegistry.clear();
        soLoudInstance->deinit();
    }

    void update(CoordinatesComponent receiverCoord) {
        //todo: Put new arguments to every 3D sound.
        //todo: Manual background ticking & kill sound that exists in a different dimension
        soLoudInstance->update3dAudio();
    }

    //Sound

    SoundID addSound(const char* filePath, bool stream, bool preload) {
        if (!exists(filePath) || !is_regular_file(filePath)) {
            lerr << "[Sound] File " << filePath << " not found!" << endl;
            return 0;
        }
        if (stream) {
            auto stream = make_unique<WavStream>();
            if (preload) {
                //stream.loadFileToMem();
            }
            else stream->load(filePath);
            soundRegistry.emplace(nextSoundId, move(stream));
        }
        else {
            auto wav = make_unique<Wav>();
            wav->load(filePath);
            soundRegistry.emplace(nextSoundId, move(wav));
        }
        nextSoundId++;
        return nextSoundId - 1;
    }

    bool removeSound(SoundID soundId) {
        const auto p = soundRegistry.find(soundId);
        if (p == soundRegistry.end()) return false;
        soundRegistry.erase(p);
        return true;
    }

    //SoundEvent

    EventID addEvent(SoundID soundId, float volume, float pitch, float distance, bool is2D, AttenuationAlgorithm atnl, bool useDoppler) {
        const auto p = soundRegistry.find(soundId);
        if (p == soundRegistry.end()) {
            lerr << "[Sound] SoundID " << soundId << " not found!" << endl;
            return 0;
        }
        const SoundEvent event{ soundId, volume, pitch, distance, atnl, useDoppler, is2D, 0 };
        eventRegistry.emplace(nextEventId, event);
        nextEventId++;
        return nextEventId - 1;
    }

    const SoundEvent* getEvent(EventID eventId) {
        const auto p = eventRegistry.find(eventId);
        if (p == eventRegistry.end()) return nullptr;
        return &p->second;
    }

    bool removeEvent(EventID eventId) {
        const auto p = eventRegistry.find(eventId);
        if (p == eventRegistry.end()) return false;
        eventRegistry.erase(p);
        return true;
    }

    //SoundEventPlay (`PlayInfo`)

    PlayID play(EventID eventId, CoordinatesComponent coordinate, float iniProgress, u32 loopCount) {
        const auto p = eventRegistry.find(eventId);
        if (p == eventRegistry.end()) {
            lerr << "[Sound] EventID " << eventId << " not found!" << endl;
            return 0;
        }
        const PlayInfo play{eventId, coordinate, iniProgress, loopCount};
        playInfos.emplace(nextPlayId, play);
        unique_lock lock(queueMutex);
        playStartQueue.push(nextPlayId);
        lock.unlock();
        audioSignal.notify_one();
        nextPlayId++;
        return nextPlayId - 1;
    }

    void pause(PlayID playId) {

    }

    void resume(PlayID playId, float progress) {

    }

    void stop(PlayID playId) {

    }

    //Worker thread

    static void audioLoop() {
        lout << "Audio" << flush;
        lout << "Hello from audio thread!" << endl;
        while (CherryGrove::isCGAlive) {
            //Audio thread will block on this line if the main thread is committing new play data.
            unique_lock lock(queueMutex);
            //Not necessary to introduce CVs.
            audioSignal.wait(lock, []() { return !playStartQueue.empty() || !CherryGrove::isCGAlive; });
            if (!CherryGrove::isCGAlive) break;
            const PlayID playId = playStartQueue.front();
            playStartQueue.pop();
            lock.unlock();
            const PlayInfo& play = playInfos[playId];
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
}