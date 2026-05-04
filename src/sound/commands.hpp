#pragma once
#include <glm/glm.hpp>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>

#include "../intrinsics/components/Coordinates.hpp"
#include "../intrinsics/components/Rotation.hpp"
#include "../intrinsics/components/Velocity.hpp"
#include "../util/Promise.hpp"
#include "enums.hpp"
#include "types.hpp"

namespace Sound {
    typedef uint32_t u32;
    using std::move, glm::vec3, SoLoud::Wav, SoLoud::WavStream, Util::Promise;

    //todo: Actually use the `fastPlay` parameter: Obviously it's not being used yet.
    //This flag is provided for high performance small sounds that need to be played with minimal latency, such as UI sound effects.
    //The original thought was to automatically create a paused play instance when the sound source is loaded, and when it is played, resume that instance instead of creating a new one, and create another paused instance for next run immediately. User is abstracted from this mechanism so they just need to pass the handle as usual to `play()`.
    struct AddSource {
        const char* filePath;
        bool isStream, is2D;
        Attenuation attn;
        InaudibleBehavior iabh;
        float volume, maxDistance, minDistance, dopplerFactor, rolloff;
        Promise<SoundHandle>* promise;
        bool soundSpeedDelay, fastPlay;

        AddSource(const char* filePath, bool isStream, bool is2D, Attenuation attn, InaudibleBehavior iabh, float volume, float maxDistance, float minDistance, float dopplerFactor, float rolloff, Promise<SoundHandle>* promise, bool soundSpeedDelay, bool fastPlay) noexcept : filePath(filePath), isStream(isStream), is2D(is2D), attn(attn), iabh(iabh), volume(volume), maxDistance(maxDistance), minDistance(minDistance), dopplerFactor(dopplerFactor), rolloff(rolloff), promise(promise), soundSpeedDelay(soundSpeedDelay), fastPlay(fastPlay) {}
        AddSource(const AddSource&) noexcept = default;
        ~AddSource() = default;
    };

    struct DeleteSource {
        SoundHandle h;
        Promise<bool>* promise;

        DeleteSource(SoundHandle h, Promise<bool>* promise) noexcept : h(h), promise(promise) {}
        DeleteSource(const DeleteSource&) noexcept = default;
        ~DeleteSource() = default;
    };

    struct Play {
        SoundHandle soundHandle;
        vec3 position, velocity;
        float iniProgress, pitch, playSpeed;
        u32 playCount;
        Promise<PlayHandle>* promise;

        Play(SoundHandle soundHandle, const Components::EntityCoordinates& position, const Components::Velocity& velocity, float iniProgress, float pitch, float playSpeed, Promise<PlayHandle>* promise, u32 playCount) noexcept : soundHandle(soundHandle), position(position.x, position.y, position.z), velocity(velocity.dx, velocity.dy, velocity.dz), iniProgress(iniProgress), pitch(pitch), playSpeed(playSpeed), playCount(playCount), promise(promise) {}
        Play(const Play&) noexcept = default;
        ~Play() = default;
    };

    struct Pause {
        PlayHandle h;
        Promise<bool>* promise;

        Pause(PlayHandle h, Promise<bool>* promise) noexcept : h(h), promise(promise) {}
        Pause(const Pause&) noexcept = default;
        ~Pause() = default;
    };

    struct Resume {
        PlayHandle h;
        Promise<bool>* promise;
        float progress;

        Resume(PlayHandle h, Promise<bool>* promise, float progress) noexcept : h(h), promise(promise), progress(progress) {}
        Resume(const Resume&) noexcept = default;
        ~Resume() = default;
    };

    struct Stop {
        PlayHandle h;
        Promise<bool>* promise;

        Stop(PlayHandle h, Promise<bool>* promise) noexcept : h(h), promise(promise) {}
        Stop(const Stop&) noexcept = default;
        ~Stop() = default;
    };

    struct GlobalPosition {
        Components::EntityCoordinates position;

        GlobalPosition(const Components::EntityCoordinates& position) noexcept : position(position) {}
        GlobalPosition(const GlobalPosition&) noexcept = default;
        ~GlobalPosition() = default;
    };

    struct GlobalRotation {
        Components::Rotation rotation;

        GlobalRotation(const Components::Rotation& rotation) noexcept : rotation(rotation) {}
        GlobalRotation(const GlobalRotation&) noexcept = default;
        ~GlobalRotation() = default;
    };

    struct GlobalVelocity {
        Components::Velocity velocity;

        GlobalVelocity(const Components::Velocity& velocity) noexcept : velocity(velocity) {}
        GlobalVelocity(const GlobalVelocity&) noexcept = default;
        ~GlobalVelocity() = default;
    };

    struct SourcePosition {
        PlayHandle h;
        Components::EntityCoordinates position;
        Promise<bool>* promise;

        SourcePosition(PlayHandle h, const Components::EntityCoordinates& position, Promise<bool>* promise) noexcept : h(h), position(position), promise(promise) {}
        SourcePosition(const SourcePosition&) noexcept = default;
        ~SourcePosition() = default;
    };

    struct SourceVelocity {
        PlayHandle h;
        Components::Velocity velocity;
        Promise<bool>* promise;

        SourceVelocity(PlayHandle h, const Components::Velocity& velocity, Promise<bool>* promise) noexcept : h(h), velocity(velocity), promise(promise) {}
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