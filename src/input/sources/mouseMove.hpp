#pragma once
#include <SDL3/SDL.h>

#include "../inputBase.hpp"

namespace InputHandler::MouseMove {
    struct EventData {
        union {
            MousePos pos;
            struct {
                float newX, newY;
            };
        };
        float deltaX, deltaY;

        EventData() noexcept : newX(0.0f), newY(0.0f), deltaX(0.0f), deltaY(0.0f) {}
    };

    struct Action {
        ActionInfo info;
        CallbackTemplate<Action, EventData> cb;

        //DONT EVER USE THIS!
        //Action(bool) noexcept {}

        Action(const ActionInfo& info, CallbackTemplate<Action, EventData> cb) noexcept : info(info), cb(cb) {}
    };

    const MousePos& getMousePos() noexcept;

    ActionID addMouseMove(const string& nameAndSpace, EventPriority priority, CallbackTemplate<Action, EventData> cb) noexcept;
    bool removeMouseMove(ActionID id) noexcept;

    void process(const SDL_Event& event, bool updateOnly = false) noexcept;
}