#define WITH_WINMM
#include <soloud/soloud.h>
#include <soloud/soloud_thread.h>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include <filesystem>
#include <thread>
#include <iostream>
#include <variant>
#include <condition_variable>

#include "../gameplay/base.hpp"
#include "SoLoudWrapper.hpp"

typedef uint32_t u32;
typedef uint64_t u64;
typedef u32 SoundID;
typedef u32 EventID;
typedef u64 PlayID;

using std::unordered_map, SoLoud::Soloud, std::make_unique, std::unique_ptr, std::cout, std::endl, SoLoud::Wav, SoLoud::WavStream, std::filesystem::exists, std::thread, std::queue, std::string, std::mutex, std::lock_guard, std::atomic, std::this_thread::sleep_for, std::chrono::milliseconds, std::variant, std::unique_lock, std::visit, std::decay_t, std::is_same_v, std::move, SoLoud::SO_NO_ERROR, std::condition_variable, std::vector;

namespace SoLoudWrapper {
	static void audioThreadFunc();

	atomic<bool> isAlive = true;
	Soloud* soLoudInstance;

	thread audioThread;
	queue<PlayID> playStartQueue;
	mutex queueMutex;
	condition_variable cv;

	unordered_map<SoundID, variant<unique_ptr<Wav>, unique_ptr<WavStream>>> soundRegistry;
	unordered_map<EventID, SoundEvent> eventRegistry;
	unordered_map<PlayID, PlayInfo> playInfos;
	SoundID nextSoundId = 0;
	EventID nextEventId = 0;
	PlayID nextPlayId = 0;

	void init() {
		cout << "Initializing SoLoud..." << endl;
		soLoudInstance = new Soloud();
		if (soLoudInstance->init(Soloud::LEFT_HANDED_3D) != SO_NO_ERROR) throw ERROR_INIT;
		cout << "SoLoud is running on " << soLoudInstance->getBackendString() << " with " << soLoudInstance->getBackendChannels() << " channels." << endl;
		cout << "Creating worker thread..." << endl;
		audioThread = thread(&audioThreadFunc);
	}

	void shutdown() {
		isAlive = false;
		cv.notify_one();
		audioThread.join();
		soundRegistry.clear();
		soLoudInstance->deinit();
	}

	void update(DiCoord receiverCoord) {
		//todo: Put new arguments to every 3D sound.
		//todo: Manual background ticking & kill sound that exists in a different dimension
		soLoudInstance->update3dAudio();
	}

	//Sound

	SoundID addSound(const char* filePath, bool stream, bool preload) {
		if (!exists(filePath)) throw ERROR_FILE_NOT_EXIST;
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

	void removeSound(SoundID soundId) {
		const auto p = soundRegistry.find(soundId);
		if (p == soundRegistry.end()) throw ERROR_SOUNDID_NOT_EXIST;
		soundRegistry.erase(p);
	}

	//SoundEvent

	EventID addEvent(SoundID soundId, float volume, float pitch, float distance, bool is2D, AttenuationAlgorithm atnl, bool useDoppler) {
		const auto p = soundRegistry.find(soundId);
		if (p == soundRegistry.end()) throw ERROR_SOUNDID_NOT_EXIST;
		const SoundEvent event{ soundId, volume, pitch, distance, atnl, useDoppler, is2D, 0 };
		eventRegistry.emplace(nextEventId, event);
		nextEventId++;
		return nextEventId - 1;
	}

	const SoundEvent* getEvent(EventID eventId) {
		const auto p = eventRegistry.find(eventId);
		//Not throwing errors if I can.
		if (p == eventRegistry.end()) return nullptr; //return ERROR_EVENTID_NOT_EXIST;
		return &(p->second);
	}

	void removeEvent(EventID eventId) {
		const auto p = eventRegistry.find(eventId);
		if (p == eventRegistry.end()) throw ERROR_EVENTID_NOT_EXIST;
		eventRegistry.erase(p);
	}

	//SoundEventPlay (`PlayInfo`)

	PlayID play(EventID eventId, DiCoord coordinate, float iniProgress, u32 loopCount) {
		const PlayInfo play{eventId, coordinate, iniProgress, loopCount};
		playInfos.emplace(nextPlayId, play);
		unique_lock lock(queueMutex);
		playStartQueue.push(nextPlayId);
		lock.unlock();
		cv.notify_one();
		nextPlayId++;
		return nextPlayId - 1;
	}

	void pause(PlayID playId) {

	}

	void resume(PlayID playId, float progress) {

	}

	void stop(PlayID playId) {

	}

	static void audioThreadFunc() {
		cout << "Hello from audio thread!" << endl;
		while (isAlive) {
			unique_lock<mutex> lock(queueMutex);
			cv.wait(lock, [] { return !playStartQueue.empty() || !isAlive; });
			if (!isAlive) break;
			const PlayID playId = playStartQueue.front();
			playStartQueue.pop();
			lock.unlock();
			const PlayInfo* play = &playInfos[playId];
			const SoundEvent* sEvent = &eventRegistry[play->eventId];
			const auto& soundInst = soundRegistry[sEvent->soundId];
			visit([&play, &sEvent](auto& soundInst) {
				if (sEvent->is2D) {
					const SoLoud::handle handle = soLoudInstance->play(*soundInst, sEvent->volume);
				}
				else {
					//todo
				}
				//Should implement background ticking manually. SoLoud seems to only provide the ability to pause and resume the sound automatically, but not resuming at a deltatime jump in progress. We need to manually jump.
			}, soundInst);
			cout << "New play request executed: Play " << playId << ", Event " << play->eventId << ", Sound " << sEvent->soundId << endl;
		}
		cout << "Terminating audio thread!" << endl;
	}
}