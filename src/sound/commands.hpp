#pragma once
#include <atomic>
#include <glm/glm.hpp>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>

#include "../components/Components.hpp"
#include "enums.hpp"
#include "types.hpp"

namespace Sound {
    typedef uint32_t u32;
    using std::atomic, std::move, glm::vec3, SoLoud::Wav, SoLoud::WavStream, Components::CoordinatesComp, Components::VelocityComp, Components::RotationComp;

    //todo: Actually use the `fastPlay` parameter: Obviously it's not being used yet.
    //This flag is provided for high performance small sounds that need to be played with minimal latency, such as UI sound effects.
    //The original thought was to automatically create a paused play instance when the sound source is loaded, and when it is played, resume that instance instead of creating a new one, and create another paused instance for next run immediately. User is abstracted from this mechanism so they just need to pass the handle as usual to `play()`.
    struct AddSource {
        const char* filePath;
        bool isStream, is2D;
        Attenuation attn;
        InaudibleBehavior iabh;
        float volume, maxDistance, minDistance, dopplerFactor, rolloff;
        SoundHandle& result;
        atomic<bool>& finished;
        bool soundSpeedDelay, fastPlay;

        //F**k C++
        AddSource(const char* filePath, bool isStream, bool is2D, Attenuation attn, InaudibleBehavior iabh, float volume, float maxDistance, float minDistance, float dopplerFactor, float rolloff, SoundHandle& result, atomic<bool>& finished, bool soundSpeedDelay, bool fastPlay) noexcept : filePath(filePath), isStream(isStream), is2D(is2D), attn(attn), iabh(iabh), volume(volume), maxDistance(maxDistance), minDistance(minDistance), dopplerFactor(dopplerFactor), rolloff(rolloff), result(result), finished(finished), soundSpeedDelay(soundSpeedDelay), fastPlay(fastPlay) {}
        AddSource(const AddSource&) noexcept = default;
        ~AddSource() = default;
    };

    struct DeleteSource {
        SoundHandle h;
        bool& success;
        atomic<bool>& finished;
        
        DeleteSource(SoundHandle h, bool& success, atomic<bool>& finished) noexcept : h(h), success(success), finished(finished) {}
        DeleteSource(const DeleteSource&) noexcept = default;
        ~DeleteSource() = default;
    };

    struct Play {
        SoundHandle soundHandle;
        vec3 position, velocity;
        float iniProgress, pitch, playSpeed;
        u32 playCount;
        PlayHandle& result;
        atomic<bool>& finished;

        Play(SoundHandle soundHandle, const CoordinatesComp& position, const VelocityComp& velocity, float iniProgress, float pitch, float playSpeed, PlayHandle& result, atomic<bool>& finished, u32 playCount) noexcept : soundHandle(soundHandle), position(position), velocity(velocity), iniProgress(iniProgress), pitch(pitch), playSpeed(playSpeed), playCount(playCount), result(result), finished(finished) {}
        Play(const Play&) noexcept = default;
        ~Play() = default;
    };

    struct Pause {
        PlayHandle h;
        bool& success;
        atomic<bool>& finished;

        Pause(PlayHandle h, bool& success, atomic<bool>& finished) noexcept : h(h), success(success), finished(finished) {}
        Pause(const Pause&) noexcept = default;
        ~Pause() = default;
    };

    struct Resume {
        PlayHandle h;
        bool& success;
        atomic<bool>& finished;
        float progress;

        Resume(PlayHandle h, bool& success, atomic<bool>& finished, float progress) noexcept : h(h), success(success), finished(finished), progress(progress) {}
        Resume(const Resume&) noexcept = default;
        ~Resume() = default;
    };

    struct Stop {
        PlayHandle h;
        bool& success;
        atomic<bool>& finished;

        Stop(PlayHandle h, bool& success, atomic<bool>& finished) noexcept : h(h), success(success), finished(finished) {}
        Stop(const Stop&) noexcept = default;
        ~Stop() = default;
    };

    struct GlobalPosition {
        const CoordinatesComp& position;

        GlobalPosition(const CoordinatesComp& position) noexcept : position(position) {}
        GlobalPosition(const GlobalPosition&) noexcept = default;
        ~GlobalPosition() = default;
    };

    struct GlobalRotation {
        const RotationComp& rotation;

        GlobalRotation(const RotationComp& rotation) noexcept : rotation(rotation) {}
        GlobalRotation(const GlobalRotation&) noexcept = default;
        ~GlobalRotation() = default;
    };

    struct GlobalVelocity {
        const VelocityComp& velocity;

        GlobalVelocity(const VelocityComp& velocity) noexcept : velocity(velocity) {}
        GlobalVelocity(const GlobalVelocity&) noexcept = default;
        ~GlobalVelocity() = default;
    };

    struct SourcePosition {
        PlayHandle h;
        const CoordinatesComp& position;
        bool& success;
        atomic<bool>& finished;

        SourcePosition(PlayHandle h, const CoordinatesComp& position, bool& success, atomic<bool>& finished) noexcept : h(h), position(position), success(success), finished(finished) {}
        SourcePosition(const SourcePosition&) noexcept = default;
        ~SourcePosition() = default;
    };

    struct SourceVelocity {
        PlayHandle h;
        const VelocityComp& velocity;
        bool& success;
        atomic<bool>& finished;

        SourceVelocity(PlayHandle h, const VelocityComp& velocity, bool& success, atomic<bool>& finished) noexcept : h(h), velocity(velocity), success(success), finished(finished) {}
        SourceVelocity(const SourceVelocity&) noexcept = default;
        ~SourceVelocity() = default;
    };

    struct Command {
        union {
            AddSource addSource;
            DeleteSource deleteSource;
            Play play;
            Pause pause;
            Resume resume;
            Stop stop;
            GlobalPosition globalPosition;
            GlobalRotation globalRotation;
            GlobalVelocity globalVelocity;
            float updateSoundSpeed;
            SourcePosition sourcePosition;
            SourceVelocity sourceVelocity;
        };
        enum struct Type : u8 {
            AddSource, DeleteSource, Play, Pause, Resume, Stop, GlobalPosition, GlobalRotation, GlobalVelocity, UpdateSoundSpeed, SourcePosition, SourceVelocity,
            Count
        } type;

        #pragma warning(suppress : 26495)
        Command() noexcept : type(Type::Count) {}

        [[nodiscard]] Command(const AddSource&      payload) noexcept : type(Type::AddSource)      { new (&addSource)      AddSource      (payload); }
        [[nodiscard]] Command(const DeleteSource&   payload) noexcept : type(Type::DeleteSource)   { new (&deleteSource)   DeleteSource   (payload); }
        [[nodiscard]] Command(const Play&           payload) noexcept : type(Type::Play)           { new (&play)           Play           (payload); }
        [[nodiscard]] Command(const Pause&          payload) noexcept : type(Type::Pause)          { new (&pause)          Pause          (payload); }
        [[nodiscard]] Command(const Resume&         payload) noexcept : type(Type::Resume)         { new (&resume)         Resume         (payload); }
        [[nodiscard]] Command(const Stop&           payload) noexcept : type(Type::Stop)           { new (&stop)           Stop           (payload); }
        [[nodiscard]] Command(const GlobalPosition& payload) noexcept : type(Type::GlobalPosition) { new (&globalPosition) GlobalPosition (payload); }
        [[nodiscard]] Command(const GlobalRotation& payload) noexcept : type(Type::GlobalRotation) { new (&globalRotation) GlobalRotation (payload); }
        [[nodiscard]] Command(const GlobalVelocity& payload) noexcept : type(Type::GlobalVelocity) { new (&globalVelocity) GlobalVelocity (payload); }
        [[nodiscard]] Command(const SourcePosition& payload) noexcept : type(Type::SourcePosition) { new (&sourcePosition) SourcePosition (payload); }
        [[nodiscard]] Command(const SourceVelocity& payload) noexcept : type(Type::SourceVelocity) { new (&sourceVelocity) SourceVelocity (payload); }
        [[nodiscard]] Command(AddSource&&      payload) noexcept : type(Type::AddSource)      { new (&addSource)      AddSource      (move(payload)); }
        [[nodiscard]] Command(DeleteSource&&   payload) noexcept : type(Type::DeleteSource)   { new (&deleteSource)   DeleteSource   (move(payload)); }
        [[nodiscard]] Command(Play&&           payload) noexcept : type(Type::Play)           { new (&play)           Play           (move(payload)); }
        [[nodiscard]] Command(Pause&&          payload) noexcept : type(Type::Pause)          { new (&pause)          Pause          (move(payload)); }
        [[nodiscard]] Command(Resume&&         payload) noexcept : type(Type::Resume)         { new (&resume)         Resume         (move(payload)); }
        [[nodiscard]] Command(Stop&&           payload) noexcept : type(Type::Stop)           { new (&stop)           Stop           (move(payload)); }
        [[nodiscard]] Command(GlobalPosition&& payload) noexcept : type(Type::GlobalPosition) { new (&globalPosition) GlobalPosition (move(payload)); }
        [[nodiscard]] Command(GlobalRotation&& payload) noexcept : type(Type::GlobalRotation) { new (&globalRotation) GlobalRotation (move(payload)); }
        [[nodiscard]] Command(GlobalVelocity&& payload) noexcept : type(Type::GlobalVelocity) { new (&globalVelocity) GlobalVelocity (move(payload)); }
        [[nodiscard]] Command(SourcePosition&& payload) noexcept : type(Type::SourcePosition) { new (&sourcePosition) SourcePosition (move(payload)); }
        [[nodiscard]] Command(SourceVelocity&& payload) noexcept : type(Type::SourceVelocity) { new (&sourceVelocity) SourceVelocity (move(payload)); }

        [[nodiscard]] Command(float payload) noexcept : type(Type::UpdateSoundSpeed) { new (&updateSoundSpeed) float (payload); }

        Command& operator=(const Command& copymove) noexcept {
            if (this == &copymove) goto same;
            switch (type) {
                case Type::AddSource:        addSource.~AddSource();           break;
                case Type::DeleteSource:     deleteSource.~DeleteSource();     break;
                case Type::Play:             play.~Play();                     break;
                case Type::Pause:            pause.~Pause();                   break;
                case Type::Resume:           resume.~Resume();                 break;
                case Type::Stop:             stop.~Stop();                     break;
                case Type::GlobalPosition:   globalPosition.~GlobalPosition(); break;
                case Type::GlobalRotation:   globalRotation.~GlobalRotation(); break;
                case Type::GlobalVelocity:   globalVelocity.~GlobalVelocity(); break;
                case Type::UpdateSoundSpeed:                                   break;
                case Type::SourcePosition:   sourcePosition.~SourcePosition(); break;
                case Type::SourceVelocity:   sourceVelocity.~SourceVelocity(); break;
                default:                                                       break;
            }
            type = copymove.type;
            switch (type) {
                case Type::AddSource:        new (&addSource)        AddSource      (copymove.addSource);        break;
                case Type::DeleteSource:     new (&deleteSource)     DeleteSource   (copymove.deleteSource);     break;
                case Type::Play:             new (&play)             Play           (copymove.play);             break;
                case Type::Pause:            new (&pause)            Pause          (copymove.pause);            break;
                case Type::Resume:           new (&resume)           Resume         (copymove.resume);           break;
                case Type::Stop:             new (&stop)             Stop           (copymove.stop);             break;
                case Type::GlobalPosition:   new (&globalPosition)   GlobalPosition (copymove.globalPosition);   break;
                case Type::GlobalRotation:   new (&globalRotation)   GlobalRotation (copymove.globalRotation);   break;
                case Type::GlobalVelocity:   new (&globalVelocity)   GlobalVelocity (copymove.globalVelocity);   break;
                case Type::UpdateSoundSpeed: new (&updateSoundSpeed) float          (copymove.updateSoundSpeed); break;
                case Type::SourcePosition:   new (&sourcePosition)   SourcePosition (copymove.sourcePosition);   break;
                case Type::SourceVelocity:   new (&sourceVelocity)   SourceVelocity (copymove.sourceVelocity);   break;
                default:                                                                                         break;
            }
            same: return *this;
        }

        ~Command() {
            switch (type) {
                case Type::AddSource:        addSource.~AddSource();           break;
                case Type::DeleteSource:     deleteSource.~DeleteSource();     break;
                case Type::Play:             play.~Play();                     break;
                case Type::Pause:            pause.~Pause();                   break;
                case Type::Resume:           resume.~Resume();                 break;
                case Type::Stop:             stop.~Stop();                     break;
                case Type::GlobalPosition:   globalPosition.~GlobalPosition(); break;
                case Type::GlobalRotation:   globalRotation.~GlobalRotation(); break;
                case Type::GlobalVelocity:   globalVelocity.~GlobalVelocity(); break;
                case Type::UpdateSoundSpeed:                                   break;
                case Type::SourcePosition:   sourcePosition.~SourcePosition(); break;
                case Type::SourceVelocity:   sourceVelocity.~SourceVelocity(); break;
                default:                                                       break;
            }
        }
    };
}