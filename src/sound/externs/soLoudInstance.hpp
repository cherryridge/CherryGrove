#pragma once
#include <memory>
#include <soloud/soloud.h>

namespace Sound {
    using std::unique_ptr, SoLoud::Soloud;

    //ONLY MEANT TO BE USED IN THE SOUND SYSTEM!
    extern unique_ptr<SoLoud::Soloud> soLoudInstance;
}