#pragma once

#include "Events.hpp"

enum struct EventOrder {
    IMMEDIATE,
    HIGH,
    NORMAL,
    LOW,
    LAST
};

#define NES_PRIORITY_TYPE EventOrder

#define NES_PRIORITY_TRAITS template<> struct nes::event_priority_traits<NES_PRIORITY_TYPE> { using priority_type = NES_PRIORITY_TYPE; static constexpr priority_type default_value = priority_type::NORMAL; };
#include <nes/event_dispatcher.hpp>

#define Listen(mod, type, listener) eventMgr.getDispatcher().listen<type, listener, EventOrder::NORMAL>(mod);
#define ListenOrdered(mod, type, listener, priority) eventMgr.getDispatcher().listen<type, listener, priority>(mod);
#define Deafen(mod, type, listener) eventMgr.getDispatcher().deafen<type, listener>(mod);

class EventManager {
private:
    nes::event_dispatcher dispatcher;
public:
    nes::event_dispatcher& getDispatcher();

    template <typename EventType>
    void trigger(EventType& event) {
        //if (Client::disable) return;
        if (!SDK::clientInstance) return;
        if (!ModuleManager::initialized) return;
        dispatcher.trigger(event);
    }
};

extern EventManager eventMgr;
