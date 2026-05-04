#pragma once
#include "ShaderDef.hpp"

#include "base.vert_dx11.h"
#include "base.vert_dx12.h"
#include "base.vert_metal.h"
#include "base.vert_opengl.h"
#include "base.vert_opengles.h"
#include "base.vert_vulkan.h"

#include "base.frag_dx11.h"
#include "base.frag_dx12.h"
#include "base.frag_metal.h"
#include "base.frag_opengl.h"
#include "base.frag_opengles.h"
#include "base.frag_vulkan.h"

namespace ShaderPool {
    inline constexpr auto base = ShaderSetDef {
        {
            base_vert_dx11, sizeof(base_vert_dx11),
            base_vert_dx12, sizeof(base_vert_dx12),
            base_vert_metal, sizeof(base_vert_metal),
            base_vert_opengl, sizeof(base_vert_opengl),
            base_vert_opengles, sizeof(base_vert_opengles),
            base_vert_vulkan, sizeof(base_vert_vulkan),
        },
        {
            base_frag_dx11, sizeof(base_frag_dx11),
            base_frag_dx12, sizeof(base_frag_dx12),
            base_frag_metal, sizeof(base_frag_metal),
            base_frag_opengl, sizeof(base_frag_opengl),
            base_frag_opengles, sizeof(base_frag_opengles),
            base_frag_vulkan, sizeof(base_frag_vulkan)
        }
    };
}