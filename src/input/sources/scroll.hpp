#pragma once
#include <SDL3/SDL.h>

#include "../inputBase.hpp"

namespace InputHandler::Scroll {
    struct EventData {
        union {
            MousePos pos;
            struct {
                float newX, newY;
            };
        };
        float scrollX, scrollY;

        EventData() : newX(0.0f), newY(0.0f), scrollX(0.0f), scrollY(0.0f) {}
    };

    struct Action {
        ActionInfo info;
        CallbackTemplate<Action, EventData> cb;

        //DONT EVER USE THIS!
        //Action(bool) noexcept {}

        Action(const ActionInfo& info, CallbackTemplate<Action, EventData> cb) noexcept : info(info), cb(cb) {}
    };

    ActionID addScroll(const string& nameAndSpace, EventPriority priority, CallbackTemplate<Action, EventData> cb) noexcept;
    bool removeScroll(ActionID id) noexcept;

    void process(const SDL_Event& event, bool updateOnly = false) noexcept;
}