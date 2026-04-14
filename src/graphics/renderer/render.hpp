#pragma once
#include <array>
#include <atomic>
#include <bgfx/bgfx.h>

#include "../../settings/Settings.hpp"
#include "../../simulation/playerEntity.hpp"
#include "../../simulation/registries.hpp"
#include "../../simulation/Simulation.hpp"
#include "../../systems/Camera.hpp"
#include "../../systems/Rotation.hpp"
#include "definitions.hpp"
#include "size.hpp"

namespace Renderer {
    using std::array, std::atomic, std::memory_order_acquire;

    inline void render() noexcept {
        array<float, 16> view, proj;
        bgfx::setViewClear(VIEWID_GAME, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x5BD093FF); //"CherryGrove Green"
        bgfx::setViewRect(VIEWID_GAME, 0, 0, internal::windowInfoCache.width, internal::windowInfoCache.height);
        if (Simulation::gameStarted.load(memory_order_acquire)) {
        //Prepare render environment
            static_cast<void>(Systems::getViewMtx(Simulation::playerEntity, view));
            static_cast<void>(Systems::getProjMtx(Simulation::playerEntity, internal::windowInfoCache.aspectRatio, proj));
            bgfx::setViewTransform(VIEWID_GAME, view.data(), proj.data());
        //Render blocks from chunk meshes
            //todo: get chunks around player (settings.renderDistance) and group the `ChunkMesh`.
            const auto renderDistance = Settings::getSettings().graphics.renderDistance;
            const auto playerPos = Simulation::registry.get<Components::EntityCoordinates>(Simulation::playerEntity);
        //Render entities
            
            //bgfx::submit();
        }
        else bgfx::touch(VIEWID_GAME);
        bgfx::frame();
    }
}