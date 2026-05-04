#pragma once

#include "gui/Gui.hpp"
#include "gui/input.hpp"
#include "renderer/prepare.hpp"
#include "renderer/render.hpp"

//vertexBuffer = createVertexBuffer(bgfx::makeRef(&blockVerticesTemplate, sizeof(blockVerticesTemplate)), layout);
//indexBuffer = createIndexBuffer(bgfx::makeRef(&blockIndicesTemplate, sizeof(blockIndicesTemplate)));

namespace Graphics {
    inline void hold() noexcept {
        while (GlobalState::isCGAlive()) {
            Renderer::prepareForFrame();
            Gui::render();
            Renderer::render();
            Gui::handleInput();
        }
    }
}