#pragma once
#include <cstdint>

namespace Sound {
    typedef uint8_t u8;

    //Order matching up with `SoLoud::AudioSource::ATTENUATION_MODELS`.
    enum struct Attenuation : u8 {
        //When: distance <= minDistance: 100%, distance >= maxDistance: 0%
        //Other: volume% = 100%
        None,
        //When: distance <= minDistance: 100%, distance >= maxDistance: 0%
        //Other: volume% = minDistance / (minDistance + rolloff * (distance - minDistance))
        //rolloff ∈ [0, +∞], > 1 recommended
        Inverse,
        //When: distance <= minDistance: 100%, distance >= maxDistance: 0%
        //Other: volume% = 1 - rolloff * (distance - minDistance) / (maxDistance - minDistance)
        //rolloff ∈ [0, 1]
        Linear,
        //When: distance <= minDistance: 100%, distance >= maxDistance: 0%
        //Other: volume% = minDistance - (distance / minDistance) ** -rolloff
        //rolloff ∈ (0, +∞]
        Exponential
    };

    //Evil bit hacks ahead. Don't reorder or reassign.
    //Essentially, 1 << 0: Whether to kill the play, 1 is kill and 0 is not kill, 1 << 1: Whether to actually pause, 0 is pause and 1 is not pause.
    enum struct InaudibleBehavior : u8 {
        Pause = 0, //Pause the audio and resume at the paused progress when audible again.
        Kill,      //Kill the audio instantly.
        PauseTick  //Pause the audio and resume at the expected progress when audible again.
    };
}