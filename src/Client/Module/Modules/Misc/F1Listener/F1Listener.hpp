#pragma once

#include "../../../../Hook/Hooks/Input/MouseHook.hpp"
#include "../../../../Client.hpp"
#include "Events/Listener.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

class F1Listener : public Listener {
public:
    void onSetTopScreenName(SetTopScreenNameEvent &event) {
        Option *hideHud = nullptr;

        hideHud = Options::getOption("dev_disableRenderHud");

        if (hideHud != nullptr && hideHud->getvalue()) {
            event.setCustomLayer("f1_screen");
        }
    }

    F1Listener() {
        Listen(this, SetTopScreenNameEvent, &F1Listener::onSetTopScreenName);
    }

    ~F1Listener() {
        Deafen(this, SetTopScreenNameEvent, &F1Listener::onSetTopScreenName);
    }
};
