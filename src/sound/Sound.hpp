#pragma once

#include "../components/Components.hpp"

namespace Sound {
	typedef uint16_t u16;
	typedef uint32_t u32;
	typedef uint64_t u64;
	typedef u32 SoundID;
	typedef u32 EventID;
	typedef u64 PlayID;

	enum AttenuationAlgorithm {
		Inverse,
		Exponential,
		Linear,
		//Volume will immediately drop to `0.0f` when the distance between source and receiver is greater than `SoundEvent::distance`.
		None
	};

	struct PlayInfo {
		EventID eventId;
		Components::CoordinatesComponent position;
		float progress;
		//`0`: Looping forever.
		u32 loopCount;
	};

	struct SoundEvent {
		SoundID soundId;
		//Maximum volume.
		float volume;
		//`1.0f` is original pitch.
		float pitch;
		//The maximum distance this sound will be played. The sound will be culled to save CPU usage if receiver goes out of distance range.
		float distance;
		AttenuationAlgorithm atnl;
		bool useDoppler;
		bool is2D;
		u16 playCount;
	};

	void init();
	void shutdown();
	void update(Components::CoordinatesComponent receiverCoord);

	SoundID addSound(const char* filePath, bool stream = false, bool preload = false);
	//Will NOT remove sound events that use the sound instance.
	//This method is NOT recommended to use.
	bool removeSound(SoundID soundId);

	EventID addEvent(SoundID soundId, float volume, float pitch, float distance, bool is2D = false, AttenuationAlgorithm atnl = AttenuationAlgorithm::Inverse, bool useDoppler = true);
	const SoundEvent* getEvent(EventID eventId);
	bool removeEvent(EventID eventId);


	PlayID play(EventID eventId, Components::CoordinatesComponent coordinate = Components::CoordinatesComponent{ 0.0f, 0.0f, 0.0f, 0 }, float iniProgress = 0.0f, u32 loopCount = 1);
	void pause(PlayID playId);
	void resume(PlayID playId, float progress);
	void stop(PlayID playId);

	void test();
};