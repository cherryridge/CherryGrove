#pragma once
#include <atomic>

#include "gui/Gui.hpp"
#include "gui/input.hpp"
#include "renderer/prepare.hpp"
#include "renderer/render.hpp"

//vertexBuffer = createVertexBuffer(bgfx::makeRef(&blockVerticesTemplate, sizeof(blockVerticesTemplate)), layout);
//indexBuffer = createIndexBuffer(bgfx::makeRef(&blockIndicesTemplate, sizeof(blockIndicesTemplate)));

namespace Graphics {
    using std::memory_order_acquire;
    
    inline void hold() noexcept {
        while (GlobalState::isCGAlive.load(memory_order_acquire)) {
            Renderer::prepareForFrame();
            Gui::render();
            Renderer::render();
            Gui::handleInput();
        }
    }
}