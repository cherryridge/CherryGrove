#pragma once

#include "../SlotTableForAudio.hpp"
#include "../types.hpp"
#include "../SoundSource.hpp"

namespace Sound {
    extern SlotTableForAudio<SoundHandle, SoundSource> soundRegistry;
}