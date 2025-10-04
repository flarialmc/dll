#pragma once

#include "../../../../Hook/Hooks/Input/MouseHook.hpp"
#include "../../../../Client.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

class CentreCursorListener : public Listener {
public:
    void onTick(TickEvent &event) {
        Client::centerCursor();
    }

    CentreCursorListener() {
        Listen(this, TickEvent, &CentreCursorListener::onTick);
    }

    ~CentreCursorListener() {
        Deafen(this, TickEvent, &CentreCursorListener::onTick);
    }
};
