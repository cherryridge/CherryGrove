#pragma once

#include "PlayInfo.hpp"
#include "SlotTableForAudio.hpp"
#include "typesAndConstants.hpp"

namespace Sound::detail {
    inline SlotTableForAudio<PlayHandle, PlayInfo> playRegistry;
}