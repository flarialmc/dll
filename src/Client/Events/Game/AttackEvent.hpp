#pragma once
#include "../Event.hpp"
#include "../Cancellable.hpp"

// Forward declaration instead of full include
class Actor;

class AttackEvent : public Event, public Cancellable {
    Actor* actor;
public:
    Actor* getActor() {
        return this->actor; // returns a pointer to the attacked actor
    }

    explicit AttackEvent(Actor* e) {
        this->actor = e;
    }
};