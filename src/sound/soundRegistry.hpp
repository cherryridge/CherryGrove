#pragma once

#include "SlotTableForAudio.hpp"
#include "SoundSource.hpp"
#include "types.hpp"

namespace Sound::detail {
    inline SlotTableForAudio<SoundHandle, SoundSource> soundRegistry;
}