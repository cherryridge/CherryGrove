#pragma once
#include <memory>
#include <soloud/soloud.h>

namespace Sound::detail {
    using std::unique_ptr;

    inline unique_ptr<SoLoud::Soloud> soLoudInstance;
}