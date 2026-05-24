#pragma once

#include "SlotTableForAudio.hpp"
#include "SoundSource.hpp"
#include "typesAndConstants.hpp"

namespace Sound::detail {
    inline SlotTableForAudio<SoundHandle, SoundSource> soundRegistry;
}