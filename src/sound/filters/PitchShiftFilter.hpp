#pragma once
#include <cstdint>
#include <soloud/soloud.h>

namespace Sound {
	typedef uint32_t u32;
	struct PitchShiftFilterInstance;

	struct PitchShiftFilter : public SoLoud::Filter {
		float pitch {1.0f};

		SoLoud::FilterInstance* createInstance() noexcept;
	};

	struct PitchShiftFilterInstance : public SoLoud::FilterInstance {
		PitchShiftFilter* parent {nullptr};

		PitchShiftFilterInstance(PitchShiftFilter* parent) noexcept;
		void filterChannel(float* buffer, u32 sampleCount, float sampleRate, double playTime, u32 currentChannel, u32 channelCount) noexcept override;
	};
}