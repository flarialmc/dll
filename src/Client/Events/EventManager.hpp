#pragma once

#include "Events.hpp"

#define Listen(mod, type, listener) eventMgr.getDispatcher().listen<type, listener>(mod);
#define Deafen(mod, type, listener) eventMgr.getDispatcher().deafen<type, listener>(mod);

class EventManager {
private:
    nes::event_dispatcher dispatcher;
public:
    nes::event_dispatcher& getDispatcher();

    template <typename EventType>
    void trigger(EventType& event) {
        //if (Client::disable) return;
        if (!ModuleManager::initialized) return;
        dispatcher.trigger(event);
    }
};

extern EventManager eventMgr;
