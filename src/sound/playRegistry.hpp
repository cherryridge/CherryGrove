#pragma once

#include "PlayInfo.hpp"
#include "SlotTableForAudio.hpp"
#include "types.hpp"

namespace Sound::detail {
    inline SlotTableForAudio<PlayHandle, PlayInfo> playRegistry;
}