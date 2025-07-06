#pragma once
#include <atomic>
#include <limits>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <boost/unordered/unordered_flat_set.hpp>
#include <glm/glm.hpp>
#include <soloud/soloud_wav.h>
#include <soloud/soloud_wavstream.h>

#include "../debug/Logger.hpp"
#include "../components/Components.hpp"

namespace Sound {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::move, std::forward, std::clamp, std::atomic, std::atomic_ref, std::pair, std::align_val_t, std::this_thread::yield, std::numeric_limits, std::vector, std::shared_ptr, std::make_shared, std::unique_ptr, std::memory_order_relaxed, std::memory_order_acquire, std::memory_order_release, std::memory_order_acq_rel, SoLoud::Wav, SoLoud::WavStream, Components::CoordinatesComp, Components::VelocityComp, Components::RotationComp, SoLoud::SO_NO_ERROR, glm::vec3, std::ostream, boost::unordered_flat_set;
    //F**k Windows marco
    constexpr float FLOAT_INFINITY = numeric_limits<float>::max();
    constexpr CoordinatesComp DUMMY_COORD_2D = {0.0, 0.0, 0.0, 0};
    constexpr u32 MAX_DEQUEUE_PER_LOOP = 16, MAX_GC_PER_LOOP = 64;

    extern u64 performance_LoopUs;

    //Only for the header's use!! DO NOT USE THIS DIRECTLY!!
    extern unique_ptr<SoLoud::Soloud> soLoudInstance;

    union Handle {
        struct {
            u32 generation, index;
        };
        u64 handle;

        Handle(u64 handle) noexcept : handle(handle) {}
        Handle(u32 generation, u32 index) noexcept : generation(generation), index(index) {}
        bool operator==(const Handle& other) noexcept { return handle == other.handle; }
        explicit operator u64() noexcept { return handle; }

        friend Logger::Logger& operator<<(Logger::Logger& os, const Handle& data) noexcept {
            os << data.handle << " (idx " << data.index << ", gen " << data.generation << ")";
            return os;
        }

        friend ostream& operator<<(ostream& os, const Handle& data) noexcept {
            os << data.handle << " (idx " << data.index << ", gen " << data.generation << ")";
            return os;
        }
    };

    typedef Handle SoundHandle;
    typedef Handle PlayHandle;

    //It's concurrent read-safe because it's only meant to be modified by the Audio Thread.
    template <typename HandleType, typename EntryType>
    struct SlotTable {
        struct Entry {
            shared_ptr<EntryType> data;
            //Generation 0 is for INVALID handles!
            u32 generation {1};
        };

        vector<Entry> storage;
        unordered_flat_set<u32> freeList;

        SlotTable() noexcept = default;
        SlotTable(u8 originSizeMagnitude) noexcept { storage.reserve(1ull << originSizeMagnitude); }

        template<typename... Args>
        pair<HandleType, shared_ptr<EntryType>> emplace(Args&&... args) noexcept {
            u32 index = 0;
            if (freeList.empty()) {
                index = static_cast<u32>(storage.size());
                //Create an empty object: generation 1, data holding nullptr.
                storage.emplace_back();
            }
            else {
                auto begin = freeList.begin();
                index = *begin;
                freeList.erase(begin);
            }
            Entry& slot = storage[index];
            u32 generation = atomic_ref(slot.generation).load(memory_order_relaxed);
            slot.data = make_shared<EntryType>(forward<Args>(args)...);
            return {{generation, index}, slot.data};
        }

        shared_ptr<EntryType> getPtr(HandleType handle) const noexcept {
            auto generation = handle.generation, index = handle.index;
            if (index >= storage.size()) goto invalid;
            {
                const auto& slot = storage[index];
                if (atomic_ref(slot.generation).load(memory_order_acquire) != generation) goto invalid;
                return slot.data;
            }
            invalid: return {};
        }

        //It might not actually destroy the shared pointer if other places still hold it.
        //WARNING: NO OTHER CODE SHOULD EVER HOLD ANY SHARED POINTER IN THE SLOTTABLE PERSISTENTLY, OR YOU WILL LEAK THE MEMORY!
        bool destroy(Handle handle) noexcept {
            auto generation = handle.generation, index = handle.index;
            if (index >= storage.size()) return false;
            auto& slot = storage[index];
            //Only the Audio Thread is allowed to modify the data, so we can compare then add without worrying about `generation` being modified by someone else on the middle.
            if (atomic_ref(slot.generation).load(memory_order_acquire) != generation) return false;
            //Release: We need to prevent the reader threads reading this after it was freed. So we need to increment the generation first using release, then reset.
            atomic_ref(slot.generation).fetch_add(1, memory_order_release);
            slot.data.reset();
            freeList.emplace(index);
            return true;
        }
    };

    //Order matching up with `SoLoud::AudioSource::ATTENUATION_MODELS`.
    enum struct Attenuation : u8 {
        //When: distance <= minDistance: 100%, distance >= maxDistance: 0%
        //Other: volume% = 100%
        None,
        //When: distance <= minDistance: 100%, distance >= maxDistance: 0%
        //Other: volume% = minDistance / (minDistance + k * (distance - minDistance))
        //rolloff ∈ [0, +∞], > 1 recommended
        Inverse,
        //When: distance <= minDistance: 100%, distance >= maxDistance: 0%
        //Other: volume% = 1 - k * (distance - minDistance) / (maxDistance - minDistance)
        //rolloff ∈ [0, 1]
        Linear,
        //When: distance <= minDistance: 100%, distance >= maxDistance: 0%
        //Other: volume% = minDistance - (distance / minDistance) ** -rolloff
        //rolloff ∈ (0, +∞]
        Exponential
    };

    //Evil bit hacks ahead. Don't reorder.
    enum struct InaudibleBehavior : u8 {
        Pause,    //Pause the audio and resume at the paused progress when audible again.
        Kill,     //Kill the audio instantly.
        PauseTick //Pause the audio and resume at the expected progress when audible again.
    };

    struct AddSource {
        const char* filePath;
        bool isStream, is2D;
        Attenuation attn;
        InaudibleBehavior iabh;
        float volume, maxDistance, minDistance, dopplerFactor, rolloff;
        SoundHandle& result;
        atomic<bool>& finished;
        bool soundSpeedDelay;

        //F**k C++
        AddSource(const char* filePath, bool isStream, bool is2D, Attenuation attn, InaudibleBehavior iabh, float volume, float maxDistance, float minDistance, float dopplerFactor, float rolloff, SoundHandle& result, atomic<bool>& finished, bool soundSpeedDelay) noexcept : filePath(filePath), isStream(isStream), is2D(is2D), attn(attn), iabh(iabh), volume(volume), maxDistance(maxDistance), minDistance(minDistance), dopplerFactor(dopplerFactor), rolloff(rolloff), result(result), finished(finished), soundSpeedDelay(soundSpeedDelay) {}
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

        Command() noexcept : type(Type::Count) {}

        Command(const AddSource&      payload) noexcept : type(Type::AddSource)      { new (&addSource)      AddSource      (payload); }
        Command(const DeleteSource&   payload) noexcept : type(Type::DeleteSource)   { new (&deleteSource)   DeleteSource   (payload); }
        Command(const Play&           payload) noexcept : type(Type::Play)           { new (&play)           Play           (payload); }
        Command(const Pause&          payload) noexcept : type(Type::Pause)          { new (&pause)          Pause          (payload); }
        Command(const Resume&         payload) noexcept : type(Type::Resume)         { new (&resume)         Resume         (payload); }
        Command(const Stop&           payload) noexcept : type(Type::Stop)           { new (&stop)           Stop           (payload); }
        Command(const GlobalPosition& payload) noexcept : type(Type::GlobalPosition) { new (&globalPosition) GlobalPosition (payload); }
        Command(const GlobalRotation& payload) noexcept : type(Type::GlobalRotation) { new (&globalRotation) GlobalRotation (payload); }
        Command(const GlobalVelocity& payload) noexcept : type(Type::GlobalVelocity) { new (&globalVelocity) GlobalVelocity (payload); }
        Command(const SourcePosition& payload) noexcept : type(Type::SourcePosition) { new (&sourcePosition) SourcePosition (payload); }
        Command(const SourceVelocity& payload) noexcept : type(Type::SourceVelocity) { new (&sourceVelocity) SourceVelocity (payload); }
        Command(AddSource&&      payload) noexcept : type(Type::AddSource)      { new (&addSource)      AddSource      (move(payload)); }
        Command(DeleteSource&&   payload) noexcept : type(Type::DeleteSource)   { new (&deleteSource)   DeleteSource   (move(payload)); }
        Command(Play&&           payload) noexcept : type(Type::Play)           { new (&play)           Play           (move(payload)); }
        Command(Pause&&          payload) noexcept : type(Type::Pause)          { new (&pause)          Pause          (move(payload)); }
        Command(Resume&&         payload) noexcept : type(Type::Resume)         { new (&resume)         Resume         (move(payload)); }
        Command(Stop&&           payload) noexcept : type(Type::Stop)           { new (&stop)           Stop           (move(payload)); }
        Command(GlobalPosition&& payload) noexcept : type(Type::GlobalPosition) { new (&globalPosition) GlobalPosition (move(payload)); }
        Command(GlobalRotation&& payload) noexcept : type(Type::GlobalRotation) { new (&globalRotation) GlobalRotation (move(payload)); }
        Command(GlobalVelocity&& payload) noexcept : type(Type::GlobalVelocity) { new (&globalVelocity) GlobalVelocity (move(payload)); }
        Command(SourcePosition&& payload) noexcept : type(Type::SourcePosition) { new (&sourcePosition) SourcePosition (move(payload)); }
        Command(SourceVelocity&& payload) noexcept : type(Type::SourceVelocity) { new (&sourceVelocity) SourceVelocity (move(payload)); }

        Command(float payload) noexcept : type(Type::UpdateSoundSpeed) { new (&updateSoundSpeed) float (payload); }

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

    //MPSC concurrent queue, mainly designed by ChatGPT.
    template <typename EntryType>
    struct MPSCQueue {
    private:
        struct Entry {
            EntryType data;
            //Whether data has been pushed and is ready for consumer to consume.
            atomic<bool> ready {false};
        };

        Entry* storage;
        const u64 size, modulo;
        atomic<u64> tail {0};
        u64 head {0};

    public:
        //Defaults to 1024 entries.
        MPSCQueue(u8 sizeMagnitude = 10) noexcept : size(1ull << sizeMagnitude), modulo(size - 1) {
            storage = static_cast<Entry*>(operator new(sizeof(Entry) * size, align_val_t(alignof(Entry))));
            for (u64 i = 0; i < size; i++) new (&storage[i]) Entry{};
        }

        void enqueue(const EntryType& data) noexcept {
            u64 pos = tail.fetch_add(1, memory_order_acquire);
            Entry& entry = storage[pos & modulo];
            //The tail may loop back and clash with the head. To prevent overriding data, we need to wait for `ready` to become `false`.
            entry.ready.wait(true, memory_order_acquire);
            entry.data = data;
            entry.ready.store(true, memory_order_release);
        }

        bool dequeue(EntryType& data) noexcept {
            Entry& entry = storage[head & modulo];
            //Empty
            if (!entry.ready.load(memory_order_acquire)) return false;
            data = move(entry.data);
            entry.ready.store(false, memory_order_release);
            //Only notifying one thread is enough.
            entry.ready.notify_one();
            head++;
            return true;
        }

        ~MPSCQueue() {
            for (u64 i = 0; i < size; i++) storage[i].~Entry();
            operator delete(storage, align_val_t(alignof(Entry)));
        }
    };

    struct SoundSource {
        union {
            Wav wav;
            WavStream wavStream;
        };
        vector<PlayHandle> instances;
        bool isStream, is2D, active {false};

        SoundSource(const char* filePath, bool isStream, bool is2D, float volume, float maxDistance, float minDistance, float dopplerFactor, Attenuation attn, InaudibleBehavior iabh, float rolloff, bool soundSpeedDelay) noexcept : isStream(isStream), is2D(is2D) {
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

        SoundSource& operator=(SoundSource&& _move) noexcept {
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

        SoundSource(SoundSource&& _move) noexcept : isStream(_move.isStream), is2D(_move.is2D), active(true) {
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

    //Read-only for other files!
    extern SlotTable<SoundHandle, SoundSource> soundRegistry;

    constexpr const char* ERROR_NOT_ACTIVE = "[Sound] The play is not active!! Check it!!";
    constexpr const char* ERROR_SHOULD_BE_PLAYING = "[Sound] The play handle is invalid! Use `play` to start it first?";
    #define ASSERT_ACTIVE \
    if (!active) {\
        lerr << ERROR_NOT_ACTIVE << endl;\
        return false;\
    }
    #define ASSERT_PLAYING \
    if (!soLoudInstance->isValidVoiceHandle(instanceHandle)) {\
        lerr << ERROR_SHOULD_BE_PLAYING << endl;\
        return false;\
    }

    struct PlayInfo {
        shared_ptr<SoundSource> soundRef;
        vec3 position, velocity;
        SoLoud::handle instanceHandle {numeric_limits<SoLoud::handle>::max()};
        u32 playCount;
        //`playSpeed` will affect speed and pitch at the same time. `pitch` is reserved and only affects pitch. If you need to only adjust the speed, not the pitch, make sure they multiply to 1.0f.
        float pitch, playSpeed;
        bool active {false};

        PlayInfo(SoundHandle soundHandle, vec3 position, vec3 velocity, u32 playCount, float pitch, float playSpeed) noexcept : position(position), velocity(velocity), playCount(playCount), pitch(pitch), playSpeed(playSpeed) {
            soundRef = soundRegistry.getPtr(soundHandle);
            if (soundRef) active = true;
            else lerr << "[Sound] For whatever reason the sound " << soundHandle << " is not found." << endl;
        }

        bool play() noexcept {
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
            soundRef->instances.push_back(instanceHandle);
            return true;
        }

        bool pause() const noexcept {
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
        bool resume(float atProgress) const noexcept {
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
        bool stop() const noexcept {
            ASSERT_ACTIVE
            ASSERT_PLAYING
            soLoudInstance->stop(instanceHandle);
            return true;
        }

        //For every external readers, PLEASE check this before doing anything.
        bool playActive() const noexcept { return soLoudInstance->isValidVoiceHandle(instanceHandle); }

        ~PlayInfo() {
            if (soLoudInstance->isValidVoiceHandle(instanceHandle)) {
                lerr << "[Sound] Struct is being destroyed before stopping: " << instanceHandle << "!" << endl;
                soLoudInstance->stop(instanceHandle);
            }
        }
    };

    //Read-only for other files!
    extern SlotTable<PlayHandle, PlayInfo> playRegistry;

    void init() noexcept;
    //Must be called after `CherryGrove::isCGAlive == false`.
    void shutdown() noexcept;

    void updateListenerPosition(const CoordinatesComp& position) noexcept;
    void updateListenerRotation(const RotationComp& rotation) noexcept;
    void updateListenerVelocity(const VelocityComp& velocity) noexcept;
    void updateSoundSpeed(float speed) noexcept;

    SoundHandle addSound(const char* filePath, bool isStream, bool is2D, float volume, float maxDistance, float minDistance, float dopplerFactor = 0.0f, Attenuation attn = Attenuation::Inverse, InaudibleBehavior iabh = InaudibleBehavior::PauseTick, float rolloff = 1.0f, bool soundSpeedDelay = false) noexcept;
    bool deleteSound(SoundHandle handle) noexcept;

    PlayHandle play(SoundHandle soundHandle, const CoordinatesComp& position, const VelocityComp& velocity = {0.0, 0.0, 0.0}, u32 playCount = 1, float iniProgress = 0.0f, float pitch = 1.0f, float playSpeed = 1.0f) noexcept;
    bool pause(PlayHandle handle) noexcept;
    bool resume(PlayHandle handle, float progress = FLOAT_INFINITY) noexcept;
    bool stop(PlayHandle handle) noexcept;
    bool updateSourcePosition(PlayHandle handle, const CoordinatesComp& position) noexcept;
    bool updateSourceVelocity(PlayHandle handle, const VelocityComp& velocity) noexcept;
};