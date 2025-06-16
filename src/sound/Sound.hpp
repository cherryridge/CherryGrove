#pragma once
#include <atomic>
#include <cstddef>
#include <limits>
#include <memory>
#include <thread>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
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
    using std::move, std::atomic, std::align_val_t, std::this_thread::yield, std::numeric_limits, std::vector, std::shared_ptr, std::unique_ptr, std::make_shared, std::memory_order_acquire, std::memory_order_release, SoLoud::Wav, SoLoud::WavStream, Components::CoordinatesComp, Components::VelocityComp, glm::vec3, SoLoud::SO_NO_ERROR;

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
        operator u64() noexcept { return handle; }
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

    #define AUDIO_SOURCE_OF(soundSource) (soundSource.isStream ? soundSource.wavStream : soundSource.wav)
    struct SoundSource {
        union {
            Wav wav;
            WavStream wavStream;
        };
        bool isStream, is2D, active = false;

        SoundSource(const char* filePath, bool isStream, bool is2D, float volume, float maxDistance, float minDistance, float dopplerFactor = 0.0f, Attenuation attn = Attenuation::Inverse, InaudibleBehavior iabh = InaudibleBehavior::PauseTick, float rolloff = 1.0f, bool soundSpeedDelay = false) noexcept : isStream(isStream), is2D(is2D) {
            if (isStream) {
                new (&wavStream) WavStream();
                auto result = wavStream.load(filePath);
                if (result != SO_NO_ERROR) {
                    lerr << "[Sound] Loading from file " << filePath << " failed: " << soLoudInstance->getErrorString(result) << endl;
                    return;
                }
                active = true;
                if (!is2D) {
                    wavStream.setVolume(volume);
                    wavStream.set3dMinMaxDistance(minDistance, maxDistance);
                    wavStream.set3dDopplerFactor(dopplerFactor);
                    wavStream.set3dAttenuation(static_cast<u32>(attn), rolloff);
                    wavStream.set3dDistanceDelay(soundSpeedDelay);
                }
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
                    wav.setVolume(volume);
                    wav.set3dMinMaxDistance(minDistance, maxDistance);
                    wav.set3dDopplerFactor(dopplerFactor);
                    wav.set3dAttenuation(static_cast<u32>(attn), rolloff);
                    wav.set3dDistanceDelay(soundSpeedDelay);
                }
                wav.setInaudibleBehavior((static_cast<u8>(iabh) & 0b00000010) >> 1, static_cast<u8>(iabh) & 0b00000001);
            }
        }

        //We can't combine ctor/mtor with copy/move assignment operator when the struct has unions.
        //We can't determine whether the union is initialized if so, and we will be destructing garbage.
        //Plus: SoLoud doesn't implement move optimizations on `Wav` nor `WavStream`, so we can just implement mtor alongside with ctor.
        SoundSource& operator=(const SoundSource& copymove) noexcept {
            if (this == &copymove) goto same;
            if (isStream) wavStream.~WavStream();
            else wav.~Wav();
            isStream = copymove.isStream;
            is2D = copymove.is2D;
            active = true;
            if (isStream) new (&wavStream) WavStream(copymove.wavStream);
            else new (&wav) Wav(copymove.wav);
            same: return *this;
        }
        
        SoundSource(const SoundSource& copymove) noexcept : isStream(copymove.isStream), is2D(copymove.is2D), active(true) {
            if (this == &copymove) return;
            if (isStream) new (&wavStream) WavStream(copymove.wavStream);
            else new (&wav) Wav(copymove.wav);
        }

        ~SoundSource() {
            if (isStream) wavStream.~WavStream();
            else wav.~Wav();
        }
    };

    struct PlayInfo {
        Handle soundHandle;
        CoordinatesComp position;
        VelocityComp velocity;
        SoLoud::handle instanceHandle = numeric_limits<SoLoud::handle>::max();
        u32 playCount;
        //`playSpeed` will affect speed and pitch at the same time. `pitch` is reserved and only affects pitch. If you need to only adjust the speed, not the pitch, make sure they multiply to 1.0f.
        float pitch, playSpeed;

        PlayInfo(Handle soundHandle, const CoordinatesComp& position, const VelocityComp& velocity = { 0.0, 0.0, 0.0 }, u32 playCount = 1, float pitch = 1.0f, float playSpeed = 1.0f) noexcept : soundHandle(soundHandle), position(position), velocity(velocity), playCount(playCount), pitch(pitch), playSpeed(playSpeed) {}

        bool play() noexcept {
            if (soLoudInstance->isValidVoiceHandle(instanceHandle)) return false;
            //get sound information by id?
            instanceHandle = soLoudInstance->play();

        }

        ~PlayInfo() {
            if (soLoudInstance->isValidVoiceHandle(instanceHandle)) lerr << "[Sound] PlayInfo is destroyed before sound stops: " << instanceHandle << "!!" << endl;
        }
    };

    struct Command {
        union {
            struct AddSource {
                const char* fileName;
                Handle& result;
                bool& success;
                ~AddSource() = default;
            } addSource;
            struct DeleteSource {
                Handle h;
                bool& success;
                ~DeleteSource() = default;
            } deleteSource;
            struct Play {
                Handle h;
                CoordinatesComp coordinate;
                VelocityComp velocity;
                float iniProgress, volume, pitch, playSpeed;
                u32 loopCount;
                ~Play() = default;
            } play;
            struct Pause {
                Handle h;
                ~Pause() = default;
            } pause;
            struct Resume {
                Handle h;
                float progress;
                ~Resume() = default;
            } resume;
            struct Stop {
                Handle h;
                ~Stop() = default;
            } stop;
        };
        enum struct Type : u8 {
            AddSource, DeleteSource, Play, Pause, Resume, Stop,
            Count
        } type;

        Command() noexcept : type(Type::Count) {};
        Command(const AddSource&    payload) noexcept : type(Type::AddSource)    { new (&addSource)    AddSource    (payload); }
        Command(const DeleteSource& payload) noexcept : type(Type::DeleteSource) { new (&deleteSource) DeleteSource (payload); }
        Command(const Play&         payload) noexcept : type(Type::Play)         { new (&play)         Play         (payload); }
        Command(const Pause&        payload) noexcept : type(Type::Pause)        { new (&pause)        Pause        (payload); }
        Command(const Resume&       payload) noexcept : type(Type::Resume)       { new (&resume)       Resume       (payload); }
        Command(const Stop&         payload) noexcept : type(Type::Stop)         { new (&stop)         Stop         (payload); }
        Command(AddSource&&    payload) noexcept : type(Type::AddSource)    { new (&addSource)    AddSource    (move(payload)); }
        Command(DeleteSource&& payload) noexcept : type(Type::DeleteSource) { new (&deleteSource) DeleteSource (move(payload)); }
        Command(Play&&         payload) noexcept : type(Type::Play)         { new (&play)         Play         (move(payload)); }
        Command(Pause&&        payload) noexcept : type(Type::Pause)        { new (&pause)        Pause        (move(payload)); }
        Command(Resume&&       payload) noexcept : type(Type::Resume)       { new (&resume)       Resume       (move(payload)); }
        Command(Stop&&         payload) noexcept : type(Type::Stop)         { new (&stop)         Stop         (move(payload)); }

        Command& operator=(const Command& copymove) noexcept {
            if (this == &copymove) goto same;
            switch (type) {
                case Type::AddSource:    addSource.~AddSource();       break;
                case Type::DeleteSource: deleteSource.~DeleteSource(); break;
                case Type::Play:         play.~Play();                 break;
                case Type::Pause:        pause.~Pause();               break;
                case Type::Resume:       resume.~Resume();             break;
                case Type::Stop:         stop.~Stop();                 break;
                default:                                               break;
            }
            type = copymove.type;
            switch (type) {
                case Type::AddSource:    new (&addSource)    AddSource    (copymove.addSource);    break;
                case Type::DeleteSource: new (&deleteSource) DeleteSource (copymove.deleteSource); break;
                case Type::Play:         new (&play)         Play         (copymove.play);         break;
                case Type::Pause:        new (&pause)        Pause        (copymove.pause);        break;
                case Type::Resume:       new (&resume)       Resume       (copymove.resume);       break;
                case Type::Stop:         new (&stop)         Stop         (copymove.stop);         break;
                default:                                                                           break;
            }
            same: return *this;
        }

        ~Command() {
            switch (type) {
                case Type::AddSource:    addSource.~AddSource();       break;
                case Type::DeleteSource: deleteSource.~DeleteSource(); break;
                case Type::Play:         play.~Play();                 break;
                case Type::Pause:        pause.~Pause();               break;
                case Type::Resume:       resume.~Resume();             break;
                case Type::Stop:         stop.~Stop();                 break;
                default:                                               break;
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
            atomic<bool> ready{false};
        };

        Entry* storage;
        const u64 size, modulo;
        atomic<u64> tail{0};
        u64 head = 0;

    public:
        //Defaults to 1024 entries.
        MPSCQueue(u8 sizeMagnitude = 10) noexcept : size(1 << sizeMagnitude), modulo(size - 1) {
            storage = static_cast<Entry*>(operator new(size, align_val_t(alignof(Entry))));
            for (u64 i = 0; i < size; i++) new (&storage[i]) Entry{};
        }

        void enqueue(const EntryType& data) noexcept {
            u64 pos = tail.fetch_add(1, memory_order_acquire);
            Entry& entry = storage[pos & modulo];
            //The tail may loop back and clash with the head. To prevent overriding data, we need to wait for `ready` to become `false`.
            entry.ready.wait(false, memory_order_acquire);
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

    //It's NOT thread-safe because it's only meant to be modified by the Audio Thread!
    template <typename EntryType>
    struct SlotTable {
    private:
        struct Entry {
            shared_ptr<EntryType> data;
            atomic<u32> generation = 1;
        };

        vector<Entry> storage;
        vector<u32> freeList;

    public:
        SlotTable() noexcept = default;
        SlotTable(u8 originSizeMagnitude) noexcept { storage.reserve(static_cast<u64>(1) << originSizeMagnitude); }

        Handle add(shared_ptr<EntryType> data) noexcept {
            while (true) {
                auto _freeHead = freeHead.load(memory_order_acquire);
                if (_freeHead == numeric_limits<u32>::max()) {
                    
                    storage.emplace_back();
                }
                else while (true) {
                    if () {

                    }
                    _freeHead = freeHead.load(memory_order_acquire);
                }
            }
        }

        shared_ptr<EntryType> getPtr(Handle handle) const noexcept {
            if (storage[handle.index].generation != handle.generation) {

            }
        }

        bool destroy(Handle handle) noexcept {
            
        }
    };

    void init() noexcept;
    void updateReceiverCoords(const CoordinatesComp& newCoords) noexcept;
    void updateSoundSpeed(float newSpeed) noexcept;
    //Must be called after `CherryGrove::isCGAlive == false`.
    void shutdown() noexcept;

    SoundID addSound(const char* filePath, bool stream) noexcept;


};