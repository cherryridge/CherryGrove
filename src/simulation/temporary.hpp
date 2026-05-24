#pragma once
#include <flecs.h>

#include "../graphics/gui/Gui.hpp"
#include "../input/boolInput/boolInput.hpp"
#include "../input/mouseMove/mouseMove.hpp"
#include "../input/pointerLock.hpp"
#include "../intrinsics/actions/ChangeRotation.hpp"
#include "../intrinsics/actions/Movement.hpp"
#include "../intrinsics/components/Camera.hpp"
#include "../intrinsics/components/Coordinates.hpp"
#include "../intrinsics/components/EntityMovementProperties.hpp"
#include "../intrinsics/components/Rotation.hpp"
#include "registries.hpp"

namespace Simulation::TemporaryCode {
    using InputHandler::BoolInput::BoolInputKind, InputHandler::MouseMove::SubKind;

    inline InputHandler::ActionID forward, backward, left, right, up, down, moveCamera;

    inline void startGame() noexcept {
        using InputHandler::BoolInput::add, InputHandler::BoolInput::BIInputSource::Keyboard, InputHandler::BoolInput::KeyCombo, InputHandler::BoolInput::addBinding;
        forward = add(IntrinsicInput::forward, 10, {BoolInputKind::Persist});
        static_cast<void>(addBinding(forward, KeyCombo(Keyboard, SDL_SCANCODE_W)));
        backward = add(IntrinsicInput::backward, 10, {BoolInputKind::Persist});
        static_cast<void>(addBinding(backward, KeyCombo(Keyboard, SDL_SCANCODE_S)));
        left = add(IntrinsicInput::left, 10, {BoolInputKind::Persist});
        static_cast<void>(addBinding(left, KeyCombo(Keyboard, SDL_SCANCODE_A)));
        right = add(IntrinsicInput::right, 10, {BoolInputKind::Persist});
        static_cast<void>(addBinding(right, KeyCombo(Keyboard, SDL_SCANCODE_D)));
        up = add(IntrinsicInput::up, 10, {BoolInputKind::Persist});
        static_cast<void>(addBinding(up, KeyCombo(Keyboard, SDL_SCANCODE_SPACE)));
        down = add(IntrinsicInput::down, 10, {BoolInputKind::Persist});
        static_cast<void>(addBinding(down, KeyCombo(Keyboard, SDL_SCANCODE_LSHIFT)));
        moveCamera = InputHandler::MouseMove::add(IntrinsicInput::changeRotationCB, 10, {SubKind::Trigger});
        InputHandler::setPointerLocked(true);

        Gui::setVisibility(Gui::Intrinsics::MainMenu, false);
        Gui::setVisibility(Gui::Intrinsics::Copyright, false);
        Gui::setVisibility(Gui::Intrinsics::Version, false);
        Gui::setVisibility(Gui::Intrinsics::DebugMenu, true);

        playerEntity = registry.entity()
            .set<Components::Camera>({60.0f})
            .set<Components::EntityCoordinates>({-0.2, -0.5, 1.0, 0u})
            .set<Components::EntityMovementProperties>({true, true, true, 1.0f, 0.2f, 0.5f})
            .set<Components::Rotation>({90.0, 0.0});
    }

    inline void exitGame() noexcept {
        Gui::setVisibility(Gui::Intrinsics::DebugMenu, false);
        Gui::setVisibility(Gui::Intrinsics::MainMenu, true);
        Gui::setVisibility(Gui::Intrinsics::Copyright, true);
        Gui::setVisibility(Gui::Intrinsics::Version, true);

        //fixme: Implement the `canDelete` mechanism properly.
        static_cast<void>(InputHandler::BoolInput::remove(forward));
        static_cast<void>(InputHandler::BoolInput::remove(backward));
        static_cast<void>(InputHandler::BoolInput::remove(left));
        static_cast<void>(InputHandler::BoolInput::remove(right));
        static_cast<void>(InputHandler::BoolInput::remove(up));
        static_cast<void>(InputHandler::BoolInput::remove(down));
        static_cast<void>(InputHandler::MouseMove::remove(moveCamera));
        InputHandler::setPointerLocked(false);

        registry.reset();
        playerEntity = flecs::entity();
    }
}