#pragma once

#include "uniforms.hpp"

namespace TexturePool {
    

    inline void init() noexcept {
        detail::initUniforms();
    }

    inline void shutdown() noexcept {
        detail::destroyUniforms();
    }


    inline 
}