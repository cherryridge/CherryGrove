#pragma once
#include <algorithm>
#include <limits>
#include <memory>
#include <glm/glm.hpp>
#include <soloud/soloud.h>

#include "externs/soundRegistry.hpp"
#include "SoundSource.hpp"
#include "types.hpp"

namespace Sound {
    typedef uint32_t u32;
    using std::shared_ptr, glm::vec3, SoLoud::handle, std::numeric_limits, std::clamp;

    inline constexpr const char* NOT_ACTIVE_STR = "[Sound] The play is not active!! Check it!!";
    #define ASSERT_ACTIVE \
    if (!active) { \
        lerr << NOT_ACTIVE_STR << endl; \
        return false; \
    }
    inline constexpr const char* NOT_PLAYING_STR = "[Sound] The play handle is invalid! Use `play` to start it first?";
    #define ASSERT_PLAYING \
    if (!soLoudInstance->isValidVoiceHandle(instanceHandle)) { \
        lerr << NOT_PLAYING_STR << endl; \
        return false; \
    }

    struct PlayInfo {
        shared_ptr<SoundSource> soundRef;
        vec3 position, velocity;
        SoLoud::handle instanceHandle{numeric_limits<SoLoud::handle>::max()};
        u32 playCount;
        //`playSpeed` will affect speed and pitch at the same time. `pitch` is reserved and only affects pitch. If you need to only adjust the speed, not the pitch, make sure they multiply to 1.0f.
        float pitch, playSpeed;
        bool active{false};

        [[nodiscard]] PlayInfo(SoundHandle soundHandle, vec3 position, vec3 velocity, u32 playCount, float pitch, float playSpeed) noexcept : position(position), velocity(velocity), playCount(playCount), pitch(pitch), playSpeed(playSpeed) {
            soundRef = soundRegistry.getPtr(soundHandle);
            if (soundRef) active = true;
            else lerr << "[Sound] For whatever reason the sound " << soundHandle.value << " is not found." << endl;
        }

        [[nodiscard]] bool play() noexcept {
            ASSERT_ACTIVE
            if (soundRef->is2D) {
                if (soundRef->isStream) instanceHandle = soLoudInstance->playBackground(soundRef->wavStream);
                else instanceHandle = soLoudInstance->playBackground(soundRef->wav);
            }
            else {
                if (soundRef->isStream) instanceHandle = soLoudInstance->play3d(soundRef->wavStream, position.x, position.y, position.z, velocity.x, velocity.y, velocity.z);
                else instanceHandle = soLoudInstance->play3d(soundRef->wav, position.x, position.y, position.z, velocity.x, velocity.y, velocity.z);
            }
            if (playCount > 1) soLoudInstance->setLooping(instanceHandle, true);
            return true;
        }

        [[nodiscard]] bool pause() const noexcept {
            ASSERT_ACTIVE
            ASSERT_PLAYING
            if (soLoudInstance->getPause(instanceHandle)) {
                lerr << "[Sound] The play " << instanceHandle << " is already paused! Use `resume` to resume it!" << endl;
                return false;
            }
            else soLoudInstance->setPause(instanceHandle, true);
            return true;
        }

        //Also can be used to jump.
        [[nodiscard]] bool resume(float atProgress) const noexcept {
            ASSERT_ACTIVE
            ASSERT_PLAYING
            if (atProgress == FLOAT_INFINITY) soLoudInstance->setPause(instanceHandle, false);
            else {
                atProgress = clamp(atProgress, 0.0f, 1.0f);
                if (soundRef->isStream) {
                    auto length = soundRef->wavStream.getLength();
                    soLoudInstance->seek(instanceHandle, length * atProgress);
                }
                else {
                    auto length = soundRef->wav.getLength();
                    soLoudInstance->seek(instanceHandle, length * atProgress);
                }
                soLoudInstance->setPause(instanceHandle, false);
            }
            return true;
        }

        //Can't merge with dtor because freaking external readers might leaking memory, so we might just keep the leaked memory useful.
        [[nodiscard]] bool stop() const noexcept {
            ASSERT_ACTIVE
            ASSERT_PLAYING
            soLoudInstance->stop(instanceHandle);
            return true;
        }

        //For every external readers, PLEASE check this before doing anything.
        [[nodiscard]] bool playActive() const noexcept { return soLoudInstance->isValidVoiceHandle(instanceHandle); }

        ~PlayInfo() {
            if (soLoudInstance->isValidVoiceHandle(instanceHandle)) {
                lerr << "[Sound] Struct is being destroyed before stopping: " << instanceHandle << "!" << endl;
                soLoudInstance->stop(instanceHandle);
            }
        }
    };
}