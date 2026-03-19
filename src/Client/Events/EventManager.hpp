#pragma once

// Core event system includes
#include <SDK/SDK.hpp>
#include <Client/Module/ModuleState.hpp>

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

class EventManager {
private:
    nes::event_dispatcher dispatcher;

public:
    nes::event_dispatcher& getDispatcher() { return dispatcher; }

    /// Dispatches an event to all registered listeners; no-ops if client isn't initialized.
    template <typename EventType>
    void trigger(EventType& event) {
        if (!SDK::clientInstance) return;
        if (!ModuleState::initialized) return;
        dispatcher.trigger(event);
    }
};

extern EventManager eventMgr;

/// Subscribes a module's handler to an event type at NORMAL priority.
#define Listen(mod, type, listener) eventMgr.getDispatcher().listen<type, listener, EventOrder::NORMAL>(mod);
/// Subscribes a module's handler to an event type at a specific priority.
#define ListenOrdered(mod, type, listener, priority) eventMgr.getDispatcher().listen<type, listener, priority>(mod);
/// Unsubscribes a module's handler from an event type.
#define Deafen(mod, type, listener) eventMgr.getDispatcher().deafen<type, listener>(mod);
