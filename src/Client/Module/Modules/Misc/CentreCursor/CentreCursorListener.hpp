#pragma once

#include "../../../../Hook/Hooks/Input/MouseHook.hpp"
#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"

class CentreCursorListener : public Listener {

public:

    void onTick(TickEvent &event) override {
        Client::centerCursor();
    }

public:
    explicit CentreCursorListener(const char string[5]) {
        this->name = string;
    }
};
