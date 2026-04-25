#pragma once
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>

#include "../debug/Logger.hpp"
#include "enums.hpp"
#include "externs/soLoudInstance.hpp"

namespace Sound {
    typedef uint32_t u32;
    using std::move, SoLoud::Wav, SoLoud::WavStream, SoLoud::SO_NO_ERROR;

    struct SoundSource {
        union {
            Wav wav;
            WavStream wavStream;
        };
        bool isStream, is2D, fastPlay, active{false};

        [[nodiscard]] SoundSource(const char* filePath, bool isStream, bool is2D, float volume, float maxDistance, float minDistance, float dopplerFactor, Attenuation attn, InaudibleBehavior iabh, float rolloff, bool soundSpeedDelay, bool fastPlay) noexcept : isStream(isStream), is2D(is2D), fastPlay(fastPlay) {
            if (isStream) {
                new (&wavStream) WavStream();
                auto result = wavStream.load(filePath);
                if (result != SO_NO_ERROR) {
                    lerr << "[Sound] Loading from file " << filePath << " failed: " << soLoudInstance->getErrorString(result) << endl;
                    return;
                }
                active = true;
                if (!is2D) {
                    wavStream.set3dMinMaxDistance(minDistance, maxDistance);
                    wavStream.set3dDopplerFactor(dopplerFactor);
                    wavStream.set3dAttenuation(static_cast<u32>(attn), rolloff);
                    wavStream.set3dDistanceDelay(soundSpeedDelay);
                }
                wavStream.setVolume(volume);
                wavStream.setInaudibleBehavior((static_cast<u8>(iabh) & 0b00000010) >> 1, static_cast<u8>(iabh) & 0b00000001);
            }
            else {
                new (&wav) Wav();
                auto result = wav.load(filePath);
                if (result != SO_NO_ERROR) {
                    lerr << "[Sound] Loading from file " << filePath << " failed: " << soLoudInstance->getErrorString(result) << endl;
                    return;
                }
                active = true;
                if (!is2D) {
                    wav.set3dMinMaxDistance(minDistance, maxDistance);
                    wav.set3dDopplerFactor(dopplerFactor);
                    wav.set3dAttenuation(static_cast<u32>(attn), rolloff);
                    wav.set3dDistanceDelay(soundSpeedDelay);
                }
                wav.setVolume(volume);
                wav.setInaudibleBehavior((static_cast<u8>(iabh) & 0b00000010) >> 1, static_cast<u8>(iabh) & 0b00000001);
            }
        }

        //We can't copy `Wav` or `WavStream`.
        SoundSource& operator=(const SoundSource&) = delete;
        SoundSource(const SoundSource&) = delete;

        [[nodiscard]] SoundSource& operator=(SoundSource&& _move) noexcept {
            if (this == &_move) goto same;
            if (isStream) wavStream.~WavStream();
            else wav.~Wav();
            isStream = _move.isStream;
            is2D = _move.is2D;
            active = true;
            if (isStream) new (&wavStream) WavStream(move(_move.wavStream));
            else new (&wav) Wav(move(_move.wav));
            same: return *this;
        }

        #pragma warning(suppress : 26495)
        [[nodiscard]] SoundSource(SoundSource&& _move) noexcept : isStream(_move.isStream), is2D(_move.is2D), active(true) {
            if (this == &_move) return;
            if (isStream) new (&wavStream) WavStream(move(_move.wavStream));
            else new (&wav) Wav(move(_move.wav));
        }

        ~SoundSource() {
            if (isStream) {
                soLoudInstance->stopAudioSource(wavStream);
                wavStream.~WavStream();
            }
            else {
                soLoudInstance->stopAudioSource(wav);
                wav.~Wav();
            }
        }
    };
}