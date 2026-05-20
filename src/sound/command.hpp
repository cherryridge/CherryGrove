#pragma once
#include <glm/glm.hpp>

#include "../intrinsics/components/Coordinates.hpp"
#include "../intrinsics/components/Rotation.hpp"
#include "../intrinsics/components/Velocity.hpp"
#include "../util/Promise.hpp"
#include "typesAndConstants.hpp"

namespace Sound::detail {
    typedef uint32_t u32;
    using std::move, glm::vec3, Util::Promise, Components::EntityCoordinates, Components::Rotation, Components::Velocity;

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
    };

    struct DeleteSource {
        SoundHandle h;
        Promise<bool>* promise;
    };

    struct Play {
        SoundHandle soundHandle;
        vec3 position, velocity;
        float iniProgress, pitch, playSpeed;
        u32 playCount;
        Promise<PlayHandle>* promise;
    };

    struct Pause {
        PlayHandle h;
        Promise<bool>* promise;
    };

    struct Resume {
        PlayHandle h;
        Promise<bool>* promise;
        float progress;
    };

    struct Stop {
        PlayHandle h;
        Promise<bool>* promise;
    };

    struct GlobalPosition {
        EntityCoordinates position;
    };

    struct GlobalRotation {
        Rotation rotation;
    };

    struct GlobalVelocity {
        Velocity velocity;
    };

    struct SourcePosition {
        PlayHandle h;
        EntityCoordinates position;
        Promise<bool>* promise;
    };

    struct SourceVelocity {
        PlayHandle h;
        Velocity velocity;
        Promise<bool>* promise;
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

        Command& operator=(const Command& other) noexcept {
            if (this == &other) goto same;
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
            type = other.type;
            switch (type) {
                case Type::AddSource:        new (&addSource)        AddSource      (other.addSource);        break;
                case Type::DeleteSource:     new (&deleteSource)     DeleteSource   (other.deleteSource);     break;
                case Type::Play:             new (&play)             Play           (other.play);             break;
                case Type::Pause:            new (&pause)            Pause          (other.pause);            break;
                case Type::Resume:           new (&resume)           Resume         (other.resume);           break;
                case Type::Stop:             new (&stop)             Stop           (other.stop);             break;
                case Type::GlobalPosition:   new (&globalPosition)   GlobalPosition (other.globalPosition);   break;
                case Type::GlobalRotation:   new (&globalRotation)   GlobalRotation (other.globalRotation);   break;
                case Type::GlobalVelocity:   new (&globalVelocity)   GlobalVelocity (other.globalVelocity);   break;
                case Type::UpdateSoundSpeed: new (&updateSoundSpeed) float          (other.updateSoundSpeed); break;
                case Type::SourcePosition:   new (&sourcePosition)   SourcePosition (other.sourcePosition);   break;
                case Type::SourceVelocity:   new (&sourceVelocity)   SourceVelocity (other.sourceVelocity);   break;
                default:                                                                                      break;
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