
#pragma once
#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../SDK/Client/Actor/Actor.hpp"

class AttackEvent : public Event {
    Actor* actor;
public:
    Actor* getActor() {
        return this->actor; // returns a pointer to the attacked actor
    }

    explicit AttackEvent(Actor* e) {
        this->actor = e;
    }
};