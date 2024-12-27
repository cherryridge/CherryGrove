#define WITH_WINMM
#include <soloud/soloud.h>
#include <soloud/soloud_thread.h>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <memory>
#include <queue>
#include <mutex>
#include <filesystem>
#include <thread>
#include <iostream>
#include <variant>
#include <condition_variable>

#include "../debug/Logger.hpp"
#include "../gameplay/base.hpp"
#include "SoLoudWrapper.hpp"

typedef uint32_t u32;
typedef uint64_t u64;
typedef u32 SoundID;
typedef u32 EventID;
typedef u64 PlayID;

using std::unordered_map, SoLoud::Soloud, std::make_unique, std::unique_ptr, Logger::lout, Logger::lerr, std::endl, SoLoud::Wav, SoLoud::WavStream, std::filesystem::exists, std::thread, std::queue, std::mutex, std::atomic, std::variant, std::unique_lock, std::move, SoLoud::SO_NO_ERROR, std::condition_variable, std::get_if, std::filesystem::is_directory, std::filesystem::is_regular_file;

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
	//We are going to use `0` as invalid ID.
	SoundID nextSoundId = 1;
	EventID nextEventId = 1;
	PlayID nextPlayId = 1;

	void init() {
		lout << "Initializing SoLoud..." << endl;
		soLoudInstance = new Soloud();
		if (soLoudInstance->init(Soloud::LEFT_HANDED_3D) != SO_NO_ERROR) throw ERROR_INIT;
		lout << "SoLoud is running on " << soLoudInstance->getBackendString() << " with " << soLoudInstance->getBackendChannels() << " channels." << endl;
		lout << "Creating audio thread..." << endl;
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
		if (!exists(filePath) || !is_regular_file(filePath)) {
			lerr << "[SoLoud] SoundEngine: File " << filePath << " not found!" << endl;
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
			lerr << "[SoLoud] SoundEngine: SoundID " << soundId << " not found!" << endl;
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
		return &(p->second);
	}

	bool removeEvent(EventID eventId) {
		const auto p = eventRegistry.find(eventId);
		if (p == eventRegistry.end()) return false;
		eventRegistry.erase(p);
		return true;
	}

	//SoundEventPlay (`PlayInfo`)

	PlayID play(EventID eventId, DiCoord coordinate, float iniProgress, u32 loopCount) {
		const auto p = eventRegistry.find(eventId);
		if (p == eventRegistry.end()) {
			lerr << "[SoLoud] SoundEngine: EventID " << eventId << " not found!" << endl;
			return 0;
		}
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
		lout << "Hello from audio thread!" << endl;
		while (isAlive) {
			unique_lock<mutex> lock(queueMutex);
			cv.wait(lock, [] { return !playStartQueue.empty() || !isAlive; });
			if (!isAlive) break;
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
					//todo
				}
				//Should implement background ticking manually. SoLoud only provide the ability to pause and resume the sound automatically, but not resuming at a deltatime jump in progress. We need to manually jump.
			}, soundInst);
			lout << "Started: Play " << playId << ", Event " << play.eventId << ", Sound " << sEvent.soundId << endl;
		}
		lout << "Terminating audio thread!" << endl;
	}
}