#pragma once

#include "../SlotTableForAudio.hpp"
#include "../types.hpp"
#include "../PlayInfo.hpp"

namespace Sound {
    extern SlotTableForAudio<PlayHandle, PlayInfo> playRegistry;
}